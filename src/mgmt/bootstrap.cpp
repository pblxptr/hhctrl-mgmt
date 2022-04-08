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
#include <mgmt/board_ctrl/settings.hpp>

namespace {
  constexpr auto BoardControlServerAddress = "tcp://127.0.0.1:9590";
  constexpr auto PlatformDeviceControlServerAddress = "tcp://127.0.0.1:9596";
}

using boost::asio::use_awaitable;
using work_guard_type =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;


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
  work_guard_type work_guard(bctx.get_executor());
  auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  auto event_bus = common::event::AsyncEventBus{bctx};

  //Board Control
  // auto bc_client = mgmt::board_ctrl::BoardControlClient{bctx, zctx};
  // auto settings = mgmt::board_ctrl::Settings { .server_address = BoardControlServerAddress };
  // boost::asio::co_spawn(bctx, mgmt::board_ctrl::async_run(command_dispatcher, bc_client, settings), common::coro::rethrow);

  boost::asio::co_spawn(bctx, mgmt::board_ctrl::async_run(
    common::utils::Client{bctx, zctx, BoardControlServerAddress}
  ), common::coro::rethrow);

  // auto client = common::utils::Client{zctx, bctx, BoardControlServerAddress};
  // boost::asio::co_spawn(bctx, mgmt::board_ctrl::async_run(client),
  //   common::coro::rethrow);

  auto hatch_dev_registry = mgmt::device::DeviceRegistry<mgmt::device::HatchDevice>{};
  auto led_dev_registry = mgmt::device::DeviceRegistry<mgmt::device::LedDevice>{};
  auto board_dev_registry = mgmt::device::DeviceRegistry<mgmt::device::BoardDevice>{};

  bctx.run();
}
}
