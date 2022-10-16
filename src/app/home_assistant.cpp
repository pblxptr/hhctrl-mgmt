#include <app/home_assistant.hpp>
#include <home_assistant/device/hatch_event_handler.hpp>
#include <home_assistant/device/main_board_event_handler.hpp>
#include <home_assistant/device/temp_sensor_event_handler.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/entity_factory.hpp>

namespace mgmt::app {
  void home_assistant_init(const HomeAssistantServices& services)
  {
    static auto device_identity_provider = mgmt::home_assistant::DeviceIdentityProvider{
      services.hw_identity_store,
      services.dtree
    };
    static auto client_factory = mgmt::home_assistant::mqttc::EntityClientFactory{ services.context, services.config.entity_client_config };
    static auto entity_factory = mgmt::home_assistant::EntityFactory{ client_factory };

    // Main board dev handler
    static auto main_board_dev_event_handler = mgmt::home_assistant::device::MainBoardEventHandler{
      entity_factory,
      device_identity_provider
    };
    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>>(main_board_dev_event_handler);

    // Hatch dev handler
    static auto hatch_dev_event_handler = mgmt::home_assistant::device::HatchEventHandler{
      entity_factory,
      device_identity_provider
    };
    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

    // Temp sensor dev handler
    static auto temp_sensor_dev_event_handler = mgmt::home_assistant::device::TempSensorEventHandler{
      entity_factory,
      device_identity_provider
    };
    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
  }
} // namespace mgmt::app
