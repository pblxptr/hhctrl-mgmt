//
// Created by bielpa on 15.08.22.
//

#include <catch2/catch_all.hpp>
#include <test_support/test_setup.hpp>
#include <test_support/test_config.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>

#include "test_params.hpp"
#include "test_context.hpp"

namespace {
constexpr auto MqttServerAddressOptionName = "--tp_mqtt_server_address";
constexpr auto MqttServerPortOptionName = "--tp_mqtt_server_port";
constexpr auto NumberOfClientsOptionName = "--tp_number_of_clients";
constexpr auto NumberOfMessagesPerClientOptionName = "--tp_messages_per_client";

constexpr auto DefaultMqttServerAddress = "localhost";
constexpr auto DefaultMqttServerPort = 8123;
constexpr auto DefaultNumberOfClients = size_t{10};
constexpr auto DefaultNumberOfMessagesPerClient = size_t{100};

auto get_config()
{
  auto& test_config = TestConfig::get();
  auto server_address = test_config.option_value(MqttServerAddressOptionName);
  auto server_port = test_config.option_value(MqttServerPortOptionName);
  auto number_of_clients = test_config.option_value(NumberOfClientsOptionName);
  auto number_of_messages_per_client = test_config.option_value(NumberOfMessagesPerClientOptionName);

  return TestParams{
    .server_address = server_address
      ? *server_address
      : DefaultMqttServerAddress,
    .server_port = server_port
      ? std::stoi(*server_port)
      : DefaultMqttServerPort,
    .number_of_clients = number_of_clients
      ? std::stoi(*number_of_clients)
      : DefaultNumberOfClients,
    .number_of_messages_per_client = number_of_messages_per_client
      ? std::stoi(*number_of_messages_per_client)
      : DefaultNumberOfMessagesPerClient
  };
}
}

void test_spec_setup()
{
  auto& config = TestConfig::get();
  config.add_option(TestOption{
    .name = MqttServerAddressOptionName,
    .hint = "mqtt_server",
    .description = "MQTT Server Address, eg. 192.168.0.1, or domain dame"
  });

  config.add_option(TestOption{
    .name = MqttServerPortOptionName,
    .hint = "mqtt server port",
    .description = "MQTT Server port, eg. 8123"
  });

  config.add_option(TestOption{
    .name = NumberOfClientsOptionName,
    .hint = "number of clients",
    .description = "Number of Async clients that will be tested"
  });

  config.add_option(TestOption{
    .name = NumberOfMessagesPerClientOptionName,
    .hint = "number of messages",
    .description = "Number of messages that every client will send"
  });
}

SCENARIO("Async clients can send and receive messages")
{
  spdlog::set_level(spdlog::level::debug);

  auto config = get_config();

  spdlog::debug("MQTT Server: {}:{}", config.server_address, config.server_port);
  spdlog::debug("Clients: {}", config.number_of_clients);
  spdlog::debug("Number of messages per client: {}", config.number_of_messages_per_client);

  auto ioc = boost::asio::io_context{};
  auto context = TestContext{ioc, std::move(config)};

  context.run();
}
