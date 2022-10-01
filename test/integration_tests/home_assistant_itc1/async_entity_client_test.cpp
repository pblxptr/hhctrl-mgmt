//
// Created by bielpa on 15.08.22.
//

#include <catch2/catch_all.hpp>
#include <test_support/test_setup.hpp>
#include <test_support/test_config.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <home_assistant/mqtt/entity_client_factory.hpp>
#include "client_wrapper.hpp"

namespace {
constexpr auto MqttServerAddressOptionName = "--tp_mqtt_server_address";
constexpr auto MqttServerPortOptionName = "--tp_mqtt_server_port";
constexpr auto NumberOfClientsOptionName = "--tp_number_of_clients";
constexpr auto NumberOfMessagesPerClientOptionName = "--tp_Messagesper_client";

constexpr auto DefaultMqttServerAddress = "localhost";
constexpr auto DefaultMqttServerPort = 8123;
constexpr auto DefaultNumberOfClients = size_t{ 10 };
constexpr auto DefaultNumberOfMessagesPerClient = size_t{ 100 };

struct MessageInfo
{
  size_t sent_for{};
  size_t received_by{};
};

auto Params = test::TestParams{};
auto Messages = std::unordered_map<std::string, MessageInfo>{};
auto Clients = std::vector<std::unique_ptr<ClientWrapper>>{};
auto IoCtx = boost::asio::io_context{};

auto set_config()
{
  auto& test_config = TestConfig::get();
  auto server_address = test_config.option_value(MqttServerAddressOptionName);
  auto server_port = test_config.option_value(MqttServerPortOptionName);
  auto number_of_clients = test_config.option_value(NumberOfClientsOptionName);
  auto number_of_messages_per_client = test_config.option_value(NumberOfMessagesPerClientOptionName);

  Params = test::TestParams{
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
  std::string build_client_id(size_t i)
  {
    return fmt::format("client-{}", i);
  }

  void run_clients()
  {
    for (auto& client : Clients) {
      boost::asio::co_spawn(IoCtx, client->async_start(), common::coro::rethrow);
    }
  }

  bool can_finish()
  {
    size_t counter = std::accumulate(Messages.begin(), Messages.end(), 0, [](size_t value, auto&& v) {
      return value + v.second.sent_for + v.second.received_by;
    });

    // Messages contain sent_for, and received_by properties. Because we multiply number of clients
    // by number of messages per client, we have to multiply it by 2 in order to take received_by into account.
    if (counter == (Params.number_of_clients * Params.number_of_messages_per_client) * 2) {
      return true;
    }

    return false;
  }

  void finish()
  {
    IoCtx.stop();
  }
}// namespace

void test_spec_setup()
{
  auto& config = TestConfig::get();
  config.add_option(TestOption{
    .name = MqttServerAddressOptionName,
    .hint = "mqtt_server",
    .description = "MQTT Server Address, eg. 192.168.0.1, or domain dame" });

  config.add_option(TestOption{
    .name = MqttServerPortOptionName,
    .hint = "mqtt server port",
    .description = "MQTT Server port, eg. 8123" });

  config.add_option(TestOption{
    .name = NumberOfClientsOptionName,
    .hint = "number of clients",
    .description = "Number of Async clients that will be tested" });

  config.add_option(TestOption{
    .name = NumberOfMessagesPerClientOptionName,
    .hint = "number of messages",
    .description = "Number of messages that every client will send" });
}

namespace test {
  std::string rand_client_id()
  {
    static auto gen = std::default_random_engine{};
    auto dist = std::uniform_int_distribution<int>(0, Params.number_of_clients - 1);

    return build_client_id(dist(gen));
  }

  size_t number_of_messages_per_client()
  {
    return Params.number_of_messages_per_client;
  }

  void fail(const std::string& client_id)
  {
    spdlog::debug("Test interrupted by client: {}", client_id);

    std::abort();
  }

  void mark_ready(const std::string& client_id)
  {
    Messages[client_id] = {};

    if (Messages.size() == Params.number_of_clients) {
      run_clients();
    }
  }
  bool marked_ready(const std::string& client_id)
  {
    return Messages.contains(client_id);
  }
  void notify_received_by(const std::string& client_id)
  {
    assert(marked_ready(client_id));

    ++Messages[client_id].received_by;

    if (can_finish()) {
      finish();
    }
  }
  void notify_sent_for(const std::string& client_id)
  {
    assert(marked_ready(client_id));

    ++Messages[client_id].sent_for;

    if (can_finish()) {
      finish();
    }
  }
}

SCENARIO("Async clients can send and receive messages")
{
  spdlog::set_level(spdlog::level::debug);

  set_config();

  spdlog::debug("MQTT Server: {}:{}", Params.server_address, Params.server_port);
  spdlog::debug("Clients: {}", Params.number_of_clients);
  spdlog::debug("Number of messages per client: {}", Params.number_of_messages_per_client);

  //Arrange
  auto config = mgmt::home_assistant::mqttc::EntityClientConfig{
    .server_address = Params.server_address,
    .server_port = Params.server_port
  };
  auto factory = mgmt::home_assistant::mqttc::EntityClientFactory{ IoCtx,
    mgmt::home_assistant::mqttc::EntityClientConfig{
      .server_address = Params.server_address,
      .server_port = Params.server_port
    }
  };

  for (std::size_t i = 0; i < Params.number_of_clients; i++) {
    auto client_id = build_client_id(i);

    Clients.push_back(std::unique_ptr<ClientWrapper>{ new AsyncClientWrapper{ factory.create_async_client(client_id) } });
  }

  //Act
  //TODO(pp): Spawn in multiple coroutines
  boost::asio::co_spawn(
    IoCtx, []() -> boost::asio::awaitable<void> {
      for (auto& client : Clients) {
        co_await client->async_connect();
      }
    },
    common::coro::rethrow);

  IoCtx.run();

  //Assert
  REQUIRE(Messages.size() == Params.number_of_clients);
  REQUIRE(Clients.size() == Params.number_of_clients);

  for (auto&& [k, v] : Messages) {
    REQUIRE(v.sent_for == v.received_by);
  }
}
