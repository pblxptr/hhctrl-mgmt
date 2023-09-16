//
// Created by bielpa on 28.08.23.
//

#include "test_support/test_config.hpp"
#include "tools.hpp"
#include <home_assistant/mqtt/logger.hpp>

void test_spec_setup()
{
  auto& config = TestConfig::get();

  config.add_option(TestOption{
    .name = MqttUsernameOptionName,
    .hint = "mqtt client username",
    .description = "MQTT client username eg. test_client_id" }
  );

  config.add_option(TestOption{
    .name = MqttPasswordOptionName,
    .hint = "mqtt client password",
    .description = "MQTT client username eg. client-name" }
  );

  config.add_option(TestOption{
    .name = MqttServerAddressOptionName,
    .hint = "mqtt_server",
    .description = "MQTT server Address, eg. 192.168.0.1, or domain dame" });

  config.add_option(TestOption{
    .name = MqttServerPortOptionName,
    .hint = "mqtt server port",
    .description = "MQTT server port, eg. 8123" });

  config.add_option(TestOption{
    .name = MqttUniqueIdOptionName,
    .hint = "mqtt client unique_id ",
    .description = "MQTT client unique_id" }
  );

    setup_logger(mgmt::home_assistant::mqtt::logger::AsyncMqttClient, spdlog::level::debug);
}
