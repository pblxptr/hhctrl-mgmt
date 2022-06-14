
#include <cassert>

#include <filesystem>
#include <vector>
#include <variant>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <common/event/event_bus.hpp>
#include <common/coro/co_spawn.hpp>

#include <cstdlib>
#include <inventory/devicetree.hpp>
#include <inventory/device_register.hpp>
#include <main_board/main_board.hpp>
#include <device/device_id.hpp>
#include <device/sysfs_led.hpp>
#include <device/sysfs_hatch.hpp>
#include <platform_device/platform_builder.hpp>
#include <platform_device/pdtree.hpp>
#include <platform_device/platform_device_provider.hpp>
#include <platform_device/hatch2sr_provider.hpp>
#include <platform_device/sysfs_rgb_indicator_provider.hpp>
#include <events/device_created.hpp>
#include <events/device_removed.hpp>


using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

struct GenericDeviceLoaderHandler
{
  mgmt::device::DeviceId_t board_id;
  mgmt::device::DeviceTree& dtree;
  common::event::AsyncEventBus& event_bus;

  template<class D, class Loader>
  bool handle(Loader&& loader) const
  {
    spdlog::get("mgmt")->debug("Loading generic device under board");

    auto device_id = loader.load();
    dtree.add_child(board_id, device_id);
    event_bus.publish(mgmt::event::DeviceCreated<D>{
      device_id
    });

    return true;
  }
};


template<
  class PlatformBuilder,
  class PlatformDeviceLoader
>
void board_init(
  PlatformBuilder builder,
  PlatformDeviceLoader platform_device_provider,
  common::event::AsyncEventBus& event_bus,
  mgmt::device::DeviceTree& dtree
)
{
  platform_device_provider.setup(builder);

  //Handle board
  auto board = std::move(builder).build_board();
  auto board_id = mgmt::device::register_device(std::move(board));
  event_bus.publish(mgmt::event::DeviceCreated<mgmt::device::MainBoard> {
    board_id
  });

  //Handler board devices
  auto generic_dev_loader_handler = GenericDeviceLoaderHandler{
    board_id,
    dtree,
    event_bus
  };
  auto generic_dev_loaders = std::move(builder).build_generic_loaders();
  for (auto&& loader : generic_dev_loaders) {
    loader(generic_dev_loader_handler);
  }
}

int main(int argc, char** argv)
{
  static auto mgmt_logger = spdlog::stdout_color_mt("mgmt");
  mgmt_logger->set_level(spdlog::level::debug);
  mgmt_logger->info("Booststrap: mgmt");

  if (argc != 2) {
    mgmt_logger->error("Too few arguments");
    return EXIT_FAILURE;
  }

  auto pdtree_path = argv[1];

  // //Messaging services
  auto bctx = boost::asio::io_context{};
  // auto zctx = zmq::context_t{};
  auto work_guard = WorkGuard_t{bctx.get_executor()};
  // auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  auto event_bus = common::event::AsyncEventBus{bctx};
  auto dtree = mgmt::device::DeviceTree{};

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device created, device id: {}", event.device_id);
      co_return;
    }
  );

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::SysfsHatch>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("SysfsHatch device created, device id: {}", event.device_id);
      co_return;
    }
  );

  board_init(
    mgmt::platform_device::PlatformBuilder<GenericDeviceLoaderHandler>{},
    mgmt::platform_device::PlatformDeviceProvider {
      pdtree_path,
      mgmt::platform_device::RGBIndicatorProvider{},
      mgmt::platform_device::HatchProvider{}
    },
    event_bus,
    dtree
  );

  boost::asio::co_spawn(bctx, []() -> boost::asio::awaitable<void> {
    auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(1);
    auto coro = co_await boost::asio::this_coro::executor;
    auto timer = boost::asio::steady_timer{coro};
    auto indicator = mgmt::device::IndicatorType::Status;

    while (true) {
      timer.expires_after(std::chrono::seconds(5));
      co_await timer.async_wait(boost::asio::use_awaitable);

      board.set_indicator_state(indicator, mgmt::device::IndicatorState::On);

      switch(indicator) {
        case mgmt::device::IndicatorType::Status:
          indicator = mgmt::device::IndicatorType::Maintenance;
        break;

        case mgmt::device::IndicatorType::Maintenance:
          indicator = mgmt::device::IndicatorType::Warning;
        break;

        case mgmt::device::IndicatorType::Warning:
          indicator = mgmt::device::IndicatorType::Fault;
        break;

        case mgmt::device::IndicatorType::Fault:
          indicator = mgmt::device::IndicatorType::Status;
        break;
      }
    }
  }, common::coro ::rethrow);


  bctx.run();
}