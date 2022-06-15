
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
#include <events/device_created.hpp>
#include <events/device_removed.hpp>
#include <app/main_board_init.hpp>
#include <app/indicator_switcher_init.hpp>


using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

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

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device created2222, device id: {}", event.device_id);
      co_return;
    }
  );

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::SysfsHatch>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("SysfsHatch device created, device id: {}", event.device_id);
      co_return;
    }
  );

  mgmt::app::indicator_switcher_init(event_bus);
  mgmt::app::main_board_init(pdtree_path, dtree, event_bus);

  bctx.run();
}