#include "bootstrap.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/this_coro.hpp>
#include <zmq.hpp>

#include <common/coro/co_spawn.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <common/command/dispatcher.hpp>
#include <common/event/event_bus.hpp>
#include <mgmt/board_ctrl/board_ctrl.hpp>
#include <mgmt/device/device_registry.hpp>
#include <mgmt/device/hatch_device.hpp>
#include <mgmt/device/led_device.hpp>
#include <mgmt/device/board_device.hpp>
#include <mgmt/board_ctrl/board.hpp>
#include <common/utils/client.hpp>
#include <mgmt/platform_device/platform_device_scanner.hpp>
#include <mgmt/platform_device/hatch_device_creator.hpp>
#include <mgmt/device/device_id.hpp>


#include <new/device/led/sysfs_led.hpp>
#include <new/device/led/rgb3_led.hpp>
#include <new/device/misc/sysfs_hatch.hpp>

#include <new2/platform_device/services/device_scaner.hpp>
#include <new2/hatch/loaders/hatch2sr_pd_loader.hpp>
#include <new/platform_device/loaders/sysfs_led_loader.hpp>
#include <new/platform_device/loaders/sysfs_rgbled_loader.hpp>

#include <new2/pooler/services/pooling_service.hpp>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9590";
  constexpr auto PlatformDeviceServerAddress = "tcp://127.0.0.1:9596";
}

using boost::asio::use_awaitable;
using work_guard_type =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;


template<class T, class...Args>
auto create_vector(Args&&... args)
{
  auto vec = std::vector<T>{};

  ( vec.push_back(std::forward<Args>(args)), ...);

  return vec;
}

namespace mgmt {

void bootstrap()
{
  static auto mgmt_logger = spdlog::stdout_color_mt("mgmt");
  mgmt_logger->set_level(spdlog::level::debug);
  mgmt_logger->info("Booststrap: mgmt");
  mgmt_logger->info("Booststrap: {}", BoardControlServerAddress);

  //Messaging services
  auto bctx = boost::asio::io_context{};
  auto zctx = zmq::context_t{};
  auto work_guard = work_guard_type{bctx.get_executor()};
  auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  auto event_bus = common::event::AsyncEventBus{bctx};

  //Pooler
  auto pooler = mgmt::pooler::PoolingService{};


  //PlatformDevice
  auto scanner = mgmt::platform_device::DeviceScanner("pdtree.json",
    create_vector<std::unique_ptr<mgmt::platform_device::DeviceCreator>>(
      std::make_unique<mgmt::platform_device::PlatformDeviceHatch2SRCreator>()
    )
  );

  bctx.run();
}

}