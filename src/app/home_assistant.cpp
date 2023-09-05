#include <app/home_assistant.hpp>
#include <home_assistant/device/hatch_event_handler.hpp>
#include <home_assistant/device/main_board_event_handler.hpp>
#include <home_assistant/device/temp_sensor_event_handler.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/entity_factory.hpp>

#include <home_assistant/event/main_board_event_handler.hpp>
#include <home_assistant/event/hatch_event_handler.hpp>
#include <home_assistant/adapter/entity_factory.hpp>
#include <home_assistant/adapter/client_factory.hpp>

namespace mgmt::app {
  void home_assistant_init(const HomeAssistantServices& services)
  {
    static auto device_identity_provider = mgmt::home_assistant::DeviceIdentityProvider{
      services.hw_identity_store,
      services.dtree
    };
    [[maybe_unused]] static auto client_factory = mgmt::home_assistant::mqttc::EntityClientFactory{ services.context, services.config.entity_client_config };
    [[maybe_unused]] static auto entity_factory = mgmt::home_assistant::EntityFactory{ client_factory };

      const auto& config = services.config.entity_client_config;
      auto client_factory_v2 = mgmt::home_assistant::adapter::MqttClientFactory{
              services.context,
              mgmt::home_assistant::adapter::BrokerConfig {
                      .username = config.username,
                      .password = config.password,
                      .host = config.server_address,
                      .port = std::to_string(config.server_port),
                      .keep_alive_interval = config.keep_alive_interval,
                      .max_reconnect_attempts = config.max_reconnect_attempts,
                      .reconnect_delay = config.reconnect_delay
              }
      };

      static auto entity_factory_v2 = mgmt::home_assistant::adapter::EntityFactory{
              std::move(client_factory_v2)
      };
    // Main board dev handler
    static auto main_board_dev_event_handler = mgmt::home_assistant::event::MainBoardEventHandler{
      entity_factory_v2,
      device_identity_provider
    };
    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::MainBoard>>(main_board_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>>(main_board_dev_event_handler);

    // Hatch dev handler
    static auto hatch_dev_event_handler = mgmt::home_assistant::event::HatchEventHandler{
      entity_factory_v2,
      device_identity_provider
    };
    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>>(hatch_dev_event_handler);
    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>>(hatch_dev_event_handler);

//    // Temp sensor dev handler
//    static auto temp_sensor_dev_event_handler = mgmt::home_assistant::device::TempSensorEventHandler{
//      entity_factory,
//      device_identity_provider
//    };
//    services.bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
//    services.bus.subscribe<mgmt::event::DeviceRemoved<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
//    services.bus.subscribe<mgmt::event::DeviceStateChanged<mgmt::device::TempSensor_t>>(temp_sensor_dev_event_handler);
  }
} // namespace mgmt::app
