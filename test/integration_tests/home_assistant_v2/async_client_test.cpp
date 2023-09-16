//
// Created by bielpa on 20.08.23.
//

#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/async_mqtt_client.hpp>

#include <boost/asio/co_spawn.hpp>
#include <thread>
#include "tools.hpp"

using mgmt::home_assistant::mqtt::AsyncMqttClient;
using mgmt::home_assistant::mqtt::Qos_t;
using mgmt::home_assistant::mqtt::PublishPacket_t;
using mgmt::home_assistant::mqtt::PublishAckPacket_t;
using mgmt::home_assistant::mqtt::SubscriptionAckPacket_t;
using mgmt::home_assistant::mqtt::ErrorCode;

constexpr auto LongIoContextRunningTimeout = std::chrono::seconds { 60 } * 30;

TEST_CASE("Client can connect to broker")
{
  auto ioc = IoContext();
  auto client = AsyncMqttClient{ioc.handle().get_executor(), get_config()};

  // NOLINTBEGIN
  boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);
    ioc.stop();

  }, rethrow);
  // NOLINTEND

  ioc.run();
}

TEST_CASE("When two clients connect with the same unique id, the first client gets disconnected")
{
    // Arrange
    auto ioc = IoContext();
    auto config = get_config();
    auto client1 = AsyncMqttClient{ioc.handle().get_executor(), config};
    auto client2 = AsyncMqttClient{ioc.handle().get_executor(), config};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client1, &client2, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        const auto c1_error = co_await client1.async_connect();
        const auto c2_error = co_await client2.async_connect();

        const auto packet = co_await client1.async_receive();

        // Assert
        REQUIRE(!c1_error);
        REQUIRE(!c2_error);
        REQUIRE(!packet);
        REQUIRE(packet.error().code() == ErrorCode::Disconnected);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

TEST_CASE("Client gets disconnected once keep alive time has passed", "[long-running]")
{
    // Arrange
    auto ioc = IoContext();
    auto config = get_config();
    config.keep_alive = 1;
    auto client = AsyncMqttClient{ioc.handle().get_executor(), config};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        const auto error_code = co_await client.async_connect();
        REQUIRE(!error_code);

        // Assert
        std::this_thread::sleep_for(std::chrono::seconds{10});
        const auto packet = co_await client.async_receive();
        REQUIRE(!packet);
        REQUIRE(packet.error().code() == ErrorCode::Disconnected);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run(LongIoContextRunningTimeout);
}

TEST_CASE("Client gets appropriate error code when resolving the name of the broker fails", "[long-running]")
{
    // Arrange
    auto ioc = IoContext();
    auto config = get_config();
    config.host = "somefakehostname_fot_test_purpose.com";
    auto client = AsyncMqttClient{ioc.handle().get_executor(), config};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        const auto error = co_await client.async_connect();

        // Assert
        REQUIRE(error);
        REQUIRE(error.code() == ErrorCode::NoService);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run(LongIoContextRunningTimeout);
}

TEST_CASE("Client gets appropriate error code when cannot connect", "[long-running]")
{
    // Arrange
    auto ioc = IoContext();
    auto config = get_config();
    config.port = "9912";
    auto client = AsyncMqttClient{ioc.handle().get_executor(), config};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        const auto error = co_await client.async_connect();

        // Assert
        REQUIRE(error);
        REQUIRE(error.code() == ErrorCode::ConnectionRefused);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run(LongIoContextRunningTimeout);
}

TEST_CASE("Client cannot publish when it is disconnected")
{
    // Arrange
    auto ioc = IoContext();
    auto client = AsyncMqttClient{ioc.handle().get_executor(), get_config()};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        const auto packet_id = co_await client.async_publish("topic", "payload", Qos_t::at_most_once);

        // Assert
        REQUIRE(!packet_id);
        REQUIRE(packet_id.error().code() == ErrorCode::Disconnected);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

TEST_CASE("Client cannot subscribe when it is disconnected")
{
    // Arrange
    auto ioc = IoContext();
    auto client = AsyncMqttClient{ioc.handle().get_executor(), get_config()};

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {

        // Act
        auto subs = std::vector<std::string>{"some-topic"};
        const auto packet_id = co_await client.async_subscribe(std::move(subs));

        // Assert
        REQUIRE(!packet_id);
        REQUIRE(packet_id.error().code() == ErrorCode::Disconnected);
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
    auto client = AsyncMqttClient{ioc.handle().get_executor(), get_config()};

    // Connect
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);

    SECTION("Client can publish messages to a topic")
    {
        SECTION("Client can publish a message to a topic")
        {
            const auto& result = co_await client.async_publish("some-topic", "some-message", Qos_t::at_most_once);
            REQUIRE(result);
        }

        SECTION("When client publishes a message with Qos_t equal to Qos_t::at_least_once it receives puback")
        {
            {
                const auto& result = co_await client.async_publish("some-topic", "some-message", Qos_t::at_least_once);
                REQUIRE(result);
            }

            {
                const auto& result = co_await client.async_receive();
                REQUIRE(result);
                REQUIRE(std::holds_alternative<PublishAckPacket_t>(result.value()));
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
          REQUIRE(std::holds_alternative<SubscriptionAckPacket_t>(result.value()));
        }

        SECTION("Client receives a message from topic it has subscribed to")
        {
            using std::to_string;

            constexpr static auto message = "some_message";
            REQUIRE((co_await client.async_publish(sub_topic, message, Qos_t::at_most_once)));

            const auto& result = co_await client.async_receive();
            REQUIRE(result);
            REQUIRE(std::holds_alternative<PublishPacket_t>(result.value()));
            const auto& payload = to_string(std::get<PublishPacket_t>(result.value()).payload());
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
