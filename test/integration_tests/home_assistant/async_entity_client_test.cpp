//
// Created by bielpa on 20.08.23.
//

#include <catch2/catch_all.hpp>

#include <test_support/test_config.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>

#include <boost/asio/co_spawn.hpp>

namespace {
  constexpr auto MqttServerAddressOptionName = "--tp_mqtt_server_address";
  constexpr auto MqttServerPortOptionName = "--tp_mqtt_server_port";

  constexpr auto DefaultMqttServerAddress = "broker.hivemq.com";
  constexpr auto DefaultMqttServerPort = "1883";
  constexpr auto DefaultUsername = "";
  constexpr auto DefaultPassword = "";

  constexpr auto LocalMqttServerAddress = "127.0.0.1";
  constexpr auto LocalMqttServerPort = "1883";
  constexpr auto LocalUsername = "test_user";
  constexpr auto LocalPassword = "test";

  constexpr auto DefaultUniqueId = "unique_id_test";

  using mgmt::home_assistant::v2::AsyncMqttClient;
  using mgmt::home_assistant::v2::ClientConfig;

  struct IoContext
  {

    void stop()
    {
      ioc_.stop();
    }

    void run()
    {
      ioc_.run();
    }

    boost::asio::io_context& handle()
    {
      return ioc_;
    }

    boost::asio::io_context ioc_ {};
    boost::asio::executor_work_guard<decltype(ioc_.get_executor())> work {ioc_.get_executor()};
  };

  auto get_broker()
  {
    return TestConfig::get().option_value(MqttServerAddressOptionName).value_or(DefaultMqttServerAddress);
  }

  auto get_port()
  {
    return TestConfig::get().option_value(MqttServerPortOptionName).value_or(DefaultMqttServerPort);
  }

  auto default_config()
  {
    return ClientConfig{
      .unique_id = DefaultUniqueId,
      .username = DefaultUsername,
      .password = DefaultPassword,
      .host = DefaultMqttServerAddress,
      .port = DefaultMqttServerPort
    };
  }

  auto local_config()
  {
    return ClientConfig{
      .unique_id = DefaultUniqueId,
      .username = LocalUsername,
      .password = LocalPassword,
      .host = LocalMqttServerAddress,
      .port = LocalMqttServerPort,
      .keep_alive = 0x1234
    };
  }

  auto rethrow(const std::exception_ptr& eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  }

} // namespace

//TEST_CASE("Client can connect to broker")
//{
//  auto ioc = IoContext();
//  auto client = AsyncMqttClient{ioc.handle().get_executor(), default_config()};
//
//  // NOLINTBEGIN
//  boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {
//    const auto error_code = co_await client.async_connect(get_broker(), get_port());
//    REQUIRE(!error_code);
//    ioc.stop();
//
//  }, rethrow);
//  // NOLINTEND
//
//  ioc.run();
//}
//
//TEST_CASE("Client can subscribe to a topic") {
//  // NOLINTBEGIN(mqtt when subscription may be rejected
//  using mgmt::home_assistant::v2::QOS;
//
//  auto ioc = IoContext{};
//
//  boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
//      auto client = AsyncMqttClient{ioc.handle().get_executor(), default_config()};
//      {
//        const auto error_code = co_await client.async_connect(get_broker(), get_port());
//        REQUIRE(!error_code);
//      }
//
//      SECTION("The number of subscription ack results matches the number of topics requested to subscribe to") {
//        static constexpr auto sub_topic = "some_topic";
//        const auto [error_code, sub_ack_results] = co_await client.async_subscribe(sub_topic, QOS::at_most_once);
//        REQUIRE(!error_code);
//        REQUIRE(sub_ack_results.size() == 1); // size == number of topics, in this case 1
//        const auto [sub_ack_topic, sub_ack_qos] = sub_ack_results[0];
//        REQUIRE(sub_ack_topic == sub_topic);
//
//        ioc.stop();
//      }
//    }, rethrow);
//
//  ioc.run();
//  // NOLINTEND
//}
//
//TEST_CASE("Client can publish messages to a topic")
//{
//  // NOLINTBEGIN(mqtt when subscription may be rejected
//  using mgmt::home_assistant::v2::QOS;
//
//  auto ioc = IoContext{};
//
//  boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
//      auto client = AsyncMqttClient{ioc.handle().get_executor(), default_config()};
//      {
//        const auto error_code = co_await client.async_connect(get_broker(), get_port());
//        REQUIRE(!error_code);
//      }
//
//      SECTION("The number of subscription ack results matches the number of topics requested to subscribe to") {
//        static constexpr auto sub_topic = "some_topic";
//        const auto [error_code, sub_ack_results] = co_await client.async_subscribe(sub_topic, QOS::at_most_once);
//        REQUIRE(!error_code);
//        REQUIRE(sub_ack_results.size() == 1); // size == number of topics, in this case 1
//        const auto [sub_ack_topic, sub_ack_qos] = sub_ack_results[0];
//        REQUIRE(sub_ack_topic == sub_topic);
//
//        ioc.stop();
//      }
//    }, rethrow);
//
//  ioc.run();
//  // NOLINTEND
//}

#include <home_assistant/mqtt/coverv2.hpp>

TEST_CASE("Cover")
{
  // NOLINTBEGIN

  using mgmt::home_assistant::v2::AsyncMqttClient;
  using mgmt::home_assistant::v2::Cover;
  using mgmt::home_assistant::v2::EntityConfig;

  auto ioc = IoContext{};
  using ClientType = AsyncMqttClient<decltype(ioc.handle().get_executor())>;

  static constexpr auto unique_id = "test_unique_id";

  spdlog::set_level(spdlog::level::debug);
  boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
    auto client = ClientType{ioc.handle().get_executor(), local_config()};
    auto cover = Cover{unique_id, std::move(client)};

    {
      const auto error_code = co_await cover.async_connect();
      REQUIRE(!error_code);
    }

    {
      const auto error_code = co_await cover.async_set_config();
    }

    const auto result = co_await cover.async_receive();


    co_return;

  }, rethrow);

  ioc.run();

  // NOLINTEND
}
