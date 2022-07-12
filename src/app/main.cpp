
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

#include <common/event/event_bus.hpp>
#include <common/coro/co_spawn.hpp>

#include <cstdlib>
#include <events/device_created.hpp>
#include <events/device_removed.hpp>
#include <events/device_state_changed.hpp>
#include <app/main_board_init.hpp>
#include <app/indicator_switcher_init.hpp>
#include <iostream>
#include <common/utils/static_map.hpp>


#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/availibility.hpp>
#include <home_assistant/device.hpp>
#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <device/hatch_t.hpp>
#include <mqtt/client.hpp>

#include <boost/asio/awaitable.hpp>
#include <home_assistant/device/hatch_event_handler.hpp>

#include <app/logger.hpp>
#include <device/logger.hpp>
#include <home_assistant/logger.hpp>

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
  setup_logger(mgmt::home_assistant::Logger, spdlog::level::debug);

  spdlog::get(mgmt::app::Logger)->info("Bootstrap mgmt");

  if (argc != 2) {
    spdlog::get("mgmt")->error("Too few arguments");

    return EXIT_FAILURE;
  }

  auto pdtree_path = argv[1];

  // //Messaging services
  auto bctx = boost::asio::io_context{};
  auto work_guard = WorkGuard_t{bctx.get_executor()};
  auto dtree = mgmt::device::DeviceTree{};
  auto bus = common::event::AsyncEventBus{bctx};

  // //Home Assistant
  auto client_factory = mgmt::home_assistant::mqttc::EntityClientFactory { bctx, "172.17.0.2", 1883 };
  auto entity_factory = mgmt::home_assistant::EntityFactory { client_factory };
  auto hatch_dev_event_handler = mgmt::home_assistant::device::HatchEventHandler{entity_factory};
  bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

  boost::asio::co_spawn(bctx, [&pdtree_path, &dtree, &bus]() -> boost::asio::awaitable<void> {
    mgmt::app::main_board_init(pdtree_path, dtree, bus);

    co_return;
  }, common::coro::rethrow);

  bctx.run();
}