
#include "device/polling_service.hpp"
#include <app/indicator_switcher_init.hpp>
#include <app/logger.hpp>
#include <app/main_board_init.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <cassert>
#include <common/coro/co_spawn.hpp>
#include <common/event/event_bus.hpp>
#include <common/utils/static_map.hpp>
#include <device/hardware_identity_store.hpp>
#include <device/hatch_t.hpp>
#include <device/logger.hpp>
#include <events/device_created.hpp>
#include <events/device_removed.hpp>
#include <events/device_state_changed.hpp>
#include <filesystem>
#include <home_assistant/availability.hpp>
#include <home_assistant/device/hatch_event_handler.hpp>
#include <home_assistant/device/main_board_event_handler.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <variant>
#include <vector>
#include <poller/logger.hpp>

using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

auto setup_logger(const std::string& logger_name, spdlog::level::level_enum level)
{
  auto logger = spdlog::stdout_color_mt(logger_name);
  logger->set_level(level);
}

int main(int argc, char** argv)
{
  setup_logger("mgmt", spdlog::level::debug);
  setup_logger(mgmt::app::Logger, spdlog::level::debug);
  setup_logger(mgmt::device::Logger, spdlog::level::debug);
  setup_logger(mgmt::poller::Logger, spdlog::level::debug);
  setup_logger(mgmt::home_assistant::Logger, spdlog::level::debug);

  spdlog::get(mgmt::app::Logger)->info("Bootstrap mgmt");

  if (argc != 2) {
    spdlog::get("mgmt")->error("Too few arguments");

    return EXIT_FAILURE;
  }

  auto pdtree_path = argv[1];

  /* General Services */
  auto bctx = boost::asio::io_context{};
  auto work_guard = WorkGuard_t{bctx.get_executor()};
  auto hw_identity_store = mgmt::device::HardwareIdentityStore_t{};
  auto dtree = mgmt::device::DeviceTree{};
  auto bus = common::event::AsyncEventBus{bctx};

  /* Home Assistant Services */
    auto device_identity_provider = mgmt::home_assistant::DeviceIdentityProvider{
    hw_identity_store,
    dtree
  };
  auto client_factory = mgmt::home_assistant::mqttc::EntityClientFactory { bctx, "192.168.0.115", 1883 };
  auto entity_factory = mgmt::home_assistant::EntityFactory { client_factory };

  //Main board dev handler
  auto main_board_dev_event_handler = mgmt::home_assistant::device::MainBoardEventHandler {
    entity_factory,
    device_identity_provider
  };
  bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(main_board_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::MainBoard>>(main_board_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>>(main_board_dev_event_handler);

  //Hatch dev handler
  auto hatch_dev_event_handler = mgmt::home_assistant::device::HatchEventHandler {
    entity_factory,
    device_identity_provider
  };
  bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

  /* Device Services */
  auto polling_service = mgmt::device::PollingService{std::ref(bctx)};
  boost::asio::co_spawn(bctx, [&pdtree_path, &dtree, &hw_identity_store, &bus, &polling_service]() -> boost::asio::awaitable<void> {
    mgmt::app::main_board_init(
      pdtree_path,
      dtree,
      hw_identity_store,
      polling_service,
      bus
    );

    co_return;
  }, common::coro::rethrow);

  bctx.run();
}