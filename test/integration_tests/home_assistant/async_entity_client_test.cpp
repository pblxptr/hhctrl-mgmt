//
// Created by bielpa on 20.08.23.
//

#include <catch2/catch_all.hpp>

#include <test_support/test_config.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>

#include <boost/asio/co_spawn.hpp>
#include "tools.hpp"

using mgmt::home_assistant::v2::AsyncMqttClient;
using mgmt::home_assistant::v2::QOS;
using mgmt::home_assistant::v2::PublishPacket;
using mgmt::home_assistant::v2::PublishAckPacket;

TEST_CASE("Client can connect to broker")
{
  auto ioc = IoContext();
  auto client = AsyncMqttClient{ioc.handle().get_executor(), default_config()};

  // NOLINTBEGIN
  boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);
    ioc.stop();

  }, rethrow);
  // NOLINTEND

  ioc.run();
}

TEST_CASE("Client can communicate with a broker")
{
  // NOLINTBEGIN
  auto ioc = IoContext{};

  boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
    auto client = AsyncMqttClient{ioc.handle().get_executor(), local_config()};

    // Connect
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);

    SECTION("Client can publish messages to a topic")
    {
        SECTION("Client can publish a message to a topic")
        {
            const auto& result = co_await client.async_publish("some-topic", "some-message", QOS::at_most_once);
        }

        SECTION("When client publishes a message with QOS equal to QOS::at_least_once it receives puback")
        {
            {
                const auto& result = co_await client.async_publish("some-topic", "some-message", QOS::at_least_once);
                REQUIRE(result);
            }

            {
                const auto& result = co_await client.async_receive();
                REQUIRE(result);
                REQUIRE(std::holds_alternative<PublishAckPacket>(result.value()));
            }
        }
    }

    SECTION("Client can subscribe to a topic and receive a message submitted to this topic")
    {
      SECTION("Client subscribes to a topic")
      {
        static constexpr auto sub_topic = "some_topic";
        auto subs = std::vector<std::string>{
            sub_topic
        };

        {
          const auto result = co_await client.async_subscribe(std::move(subs));
          REQUIRE(result);
        }

        {
          const auto result = co_await client.async_receive();
          REQUIRE(result);
        }

        SECTION("Client receives a message from topic it has subscribed to")
        {
            using std::to_string;

            constexpr static auto message = "some_message";
            REQUIRE((co_await client.async_publish(sub_topic, message, QOS::at_most_once)));

            const auto& result = co_await client.async_receive();
            REQUIRE(result);
            REQUIRE(std::holds_alternative<PublishPacket>(result.value()));
            const auto& payload = to_string(std::get<PublishPacket>(result.value()).payload());
            REQUIRE(payload == message);
        }
      }
    }

    ioc.stop();
    }, rethrow
  );

  ioc.run();
  // NOLINTEND
}
