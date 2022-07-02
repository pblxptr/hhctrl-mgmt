
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
#include <home_assistant/mqtt/availibility.hpp>
#include <home_assistant/mqtt/device.hpp>
#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/hatch_dev_handler.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <device/hatch_t.hpp>

#include <boost/asio/awaitable.hpp>

class HatchDeviceEventHandler
{
public:
  HatchDeviceEventHandler() = default;
  HatchDeviceEventHandler(const HatchDeviceEventHandler&) = delete;
  HatchDeviceEventHandler& operator=(const HatchDeviceEventHandler&) = delete;
  HatchDeviceEventHandler(HatchDeviceEventHandler&&) = default;
  HatchDeviceEventHandler& operator=(HatchDeviceEventHandler&&) = default;

  boost::asio::awaitable<void> operator()(const mgmt::event::DeviceCreated<mgmt::device::Hatch_t>& event)
  {
    co_return;
  }

  boost::asio::awaitable<void> operator()(const mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>& event)
  {
    co_return;
  }

  // boost::asio::awaitable<void> operator()()(const mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>& event)
  // {
  //   co_return;
  // }
};

using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

auto setup_logger()
{
  static auto mgmt_logger = spdlog::stdout_color_mt("mgmt");
  mgmt_logger->set_level(spdlog::level::debug);
}

int main(int argc, char** argv)
{
  setup_logger();
  spdlog::get("mgmt")->info("Bootstrap mgmt");

  if (argc != 2) {
    spdlog::get("mgmt")->error("Too few arguments");

    return EXIT_FAILURE;
  }

  auto pdtree_path = argv[1];

  // //Messaging services
  auto bctx = boost::asio::io_context{};
  auto work_guard = WorkGuard_t{bctx.get_executor()};

  auto entity_client_factory = mgmt::home_assistant::mqttc::EntityClientFactory { bctx, "172.17.0.5", 1883 };
  auto bus = common::event::AsyncEventBus{bctx};


  auto hatch_dev_event_handler = HatchDeviceEventHandler{};
  bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
  bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

  // auto hatch_dev_handler = mgmt::home_assistant::HatchDeviceHandler{entity_client_factory};


  bctx.run();
}