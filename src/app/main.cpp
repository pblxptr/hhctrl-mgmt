#include <filesystem>
#include <cassert>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <poller/polling_service.hpp>

#include <app/logger.hpp>
#include <app/main_board_init.hpp>
#include <app/app_config.hpp>
#include <home_assistant/device/hatch_event_handler.hpp>
#include <home_assistant/device/main_board_event_handler.hpp>
#include <home_assistant/device/temp_sensor_event_handler.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/entity_factory.hpp>

#include <cstdlib>
#include <iostream>

using WorkGuard_t =
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

int main(int argc, char** argv)
{
  try {
    if (argc != 2) {
      common::logger::get(mgmt::app::Logger)->error("Too few arguments");
      std::cerr << "Too few arguments.\n";
      std::cerr << fmt::format("Example: {} /path/to/config.json\n", argv[0]);

      return EXIT_FAILURE;
    }

    auto config = mgmt::app::load_config(argv[1]);
    mgmt::app::init_logger(config.log_dir);

    spdlog::get("mgmt")->info("App config: {}", pretty_format_config(config));

    /* General Services */
    auto bctx = boost::asio::io_context{};
    auto work_guard = WorkGuard_t{ bctx.get_executor() };
    auto hw_identity_store = mgmt::device::HardwareIdentityStore_t{};
    auto dtree = mgmt::device::DeviceTree{};
    auto bus = common::event::AsyncEventBus{ bctx };

    /* Home Assistant Services */
    auto device_identity_provider = mgmt::home_assistant::DeviceIdentityProvider{
      hw_identity_store,
      dtree
    };
    auto client_factory = mgmt::home_assistant::mqttc::EntityClientFactory{ bctx, config.entity_client_config };
    auto entity_factory = mgmt::home_assistant::EntityFactory{ client_factory };

    // Main board dev handler
    auto main_board_dev_event_handler = mgmt::home_assistant::device::MainBoardEventHandler{
      entity_factory,
      device_identity_provider
    };
    bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>>(main_board_dev_event_handler);

    // Hatch dev handler
    auto hatch_dev_event_handler = mgmt::home_assistant::device::HatchEventHandler{
      entity_factory,
      device_identity_provider
    };
    bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

    // Temp sensor dev handler
    auto temp_sensor_dev_event_handler = mgmt::home_assistant::device::TempSensorEventHandler{
      entity_factory,
      device_identity_provider
    };
    bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
    bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);

    /* Device Services */
    auto polling_service = mgmt::device::PollingService{ std::ref(bctx) };

    /* Run */
    boost::asio::co_spawn(
      bctx, [pdtree_path = config.dtree_file, &dtree, &hw_identity_store, &bus, &polling_service]() -> boost::asio::awaitable<void> {
        mgmt::app::main_board_init(
          pdtree_path,
          dtree,
          hw_identity_store,
          polling_service,
          bus);

        co_return;
      },
      common::coro::rethrow);
    bctx.run();
  } catch (const std::exception& exception) {
    common::logger::get(mgmt::app::Logger)->error(fmt::format("Exception has been thrown: {}", exception.what()));
    std::abort();
  }
}
