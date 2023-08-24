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
  constexpr auto DefaultMqttServerPort = "99977";
  constexpr auto DefaultUniqueId = "unique_id_test";

  constexpr auto DefaultUsername = "";
  constexpr auto DefaultPassword = "";

  using mgmt::home_assistant::v2::AsyncMqttClient;
  using mgmt::home_assistant::v2::ClientConfig;

  auto get_broker()
  {
    return TestConfig::get().option_value(MqttServerAddressOptionName).value_or(DefaultMqttServerAddress);
  }

  auto get_port()
  {
    return TestConfig::get().option_value(MqttServerPortOptionName).value_or(DefaultMqttServerPort);
  }

  auto rethrow(const std::exception_ptr& eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  }

} // namespace

TEST_CASE("Client can connect to broker")
{
  auto ioc = boost::asio::io_context{};
  [[maybe_unused]] auto work = boost::asio::make_work_guard(ioc);
  auto client = AsyncMqttClient{ioc.get_executor(),
    ClientConfig{
      .unique_id = DefaultUniqueId,
      .username = DefaultUsername,
      .password = DefaultPassword
    }
  };

  boost::asio::co_spawn(ioc, [&client, &ioc]() -> boost::asio::awaitable<void> {
    const auto error_code = co_await client.async_connect(get_broker(), get_port());
    REQUIRE(!error_code);
    ioc.stop();

  }, rethrow);

  ioc.run();
}

