//
// Created by bielpa on 20.08.23.
//

#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/async_mqtt_client.hpp>

#include <boost/asio/co_spawn.hpp>
#include <thread>
#include "tools.hpp"

using mgmt::home_assistant::v2::AsyncMqttClient;
using mgmt::home_assistant::v2::Qos_t;
using mgmt::home_assistant::v2::PublishPacket_t;
using mgmt::home_assistant::v2::PublishAckPacket_t;
using mgmt::home_assistant::v2::SubscriptionAckPacket_t;
using mgmt::home_assistant::v2::ErrorCode;

constexpr auto LongIoContextRunningTimeout = std::chrono::seconds { 60 } * 30;

TEST_CASE("Client can connect to broker")
{
  auto ioc = IoContext();
  auto client = AsyncMqttClient{ioc.handle().get_executor(), get_config()};

  // NOLINTBEGIN
  boost::asio::co_spawn(ioc.handle(), [&client, &ioc]() -> boost::asio::awaitable<void> {
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);

    co_await client.async_receive();

    ioc.stop();

  }, rethrow);
  // NOLINTEND

  ioc.run(std::chrono::seconds {90});
}
