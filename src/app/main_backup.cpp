
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
#include <home_assistant/mqtt/config_builder.hpp>
#include <iostream>

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
  auto work_guard = WorkGuard_t{ bctx.get_executor() };
  // auto command_dispatcher = common::command::AsyncCommandDispatcher{};
  auto event_bus = common::event::AsyncEventBus{ bctx };
  auto dtree = mgmt::device::DeviceTree{};

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device created, device id: {}", event.device_id);
      co_return;
    });

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device created2222, device id: {}", event.device_id);
      co_return;
    });

  event_bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::SysfsHatch>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("SysfsHatch device created, device id: {}", event.device_id);
      co_return;
    });

  auto config_builder = mgmt::home_assistant::ConfigBuilder{ "asdjkl1231l23h" };
  config_builder.set("name", "My fancy device");
  config_builder.set("device_class", "door");
  config_builder.set("command_topic", mgmt::home_assistant::mqtt_topic(mgmt::home_assistant::TopicPlaceholder::UniqueId, "state"));
  config_builder.set("command_topic", mgmt::home_assistant::mqtt_topic(mgmt::home_assistant::TopicPlaceholder::UniqueId, "set"));
  config_builder.set("availibility_topic", mgmt::home_assistant::mqtt_topic(mgmt::home_assistant::TopicPlaceholder::UniqueId, "availibitity"));
  config_builder.set("payload_open", "open");
  config_builder.set("payload_close", "close");
  config_builder.set("payload_stop", nullptr);
  config_builder.set("state_open", "open");
  config_builder.set("state_opening", "opening");
  config_builder.set("state_closed", "closed");
  config_builder.set("state_closing", "closing");
  config_builder.set("json_attributes_topic", mgmt::home_assistant::mqtt_topic(mgmt::home_assistant::TopicPlaceholder::UniqueId, "attributes"));
  config_builder.set("json_attributes_template", "{{ value_json | tojson }}");
  auto config = config_builder.build();

  // mgmt::app::indicator_switcher_init(event_bus);
  // mgmt::app::main_board_init(pdtree_path, dtree, event_bus);

  auto cover = Cover{ "unique_id", Device{}, Client{} };

  bctx.run();
}
struct Cover
{
  struct Availibility
  {
    constexpr static inline auto Topic = "availibility";
  };

  struct Command
  {
    constexpr static inline auto Topic = "cover1/set";
    constexpr static inline auto PayloadOpen = "open";
    constexpr static inline auto PayloadClose = "close";
    constexpr static inline auto PayloadStop = nullptr;
  };

  struct State
  {
    constexpr static inline auto Topic = "conver1/set";
    constexpr static inline auto StateOpen = "open";
    constexpr static inline auto StateOpening = "opening";
    constexpr static inline auto StateClose = "close";
    constexpr static inline auto StateClosing = "closing";
  };
}
