//
// Created by bielpa on 28.08.23.
//

#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>
#include "test_support/test_config.hpp"

#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

constexpr auto MqttServerAddressOptionName = "--tp_mqtt_server_address";
constexpr auto MqttServerPortOptionName = "--tp_mqtt_server_port";
constexpr auto MqttUsernameOptionName = "--username";
constexpr auto MqttPasswordOptionName = "--password";
constexpr auto MqttUniqueIdOptionName = "--unique_id";

constexpr auto DefaultMqttServerAddress = "broker.hivemq.com";
constexpr auto DefaultMqttServerPort = "1883";
constexpr auto DefaultUsername = "";
constexpr auto DefaultPassword = "";
constexpr auto DefaultUniqueId = "unique_id_test";
constexpr std::uint16_t DefaultKeepAlive = 0x1234;
constexpr auto DefaultDiscoveryTopicPrefix = "homeassistant";

constexpr auto LocalMqttServerAddress = "127.0.0.1";
constexpr auto LocalMqttServerPort = "1883";
constexpr auto LocalUsername = "test_user";
constexpr auto LocalPassword = "test";

inline auto rethrow(const std::exception_ptr& eptr)
{
    if (eptr) {
        std::rethrow_exception(eptr);
    }
}

struct IoContext
{
  void stop()
  {
    ioc_.stop();
  }

  void run(std::chrono::seconds timeout = std::chrono::seconds { 30 })
  {
//    // NOLINTBEGIN
    boost::asio::co_spawn(ioc_, [this, timeout]() -> boost::asio::awaitable<void> {
        auto timer = boost::asio::steady_timer {ioc_.get_executor()};
        timer.expires_after(timeout);

        auto ec = boost::system::error_code {};

        co_await timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        if (!ec) {
            WARN("Test timeout!");
            REQUIRE(false);
            co_return;
        }

    }, rethrow);
//    // NOLINTEND

    ioc_.run();
  }

  boost::asio::io_context& handle()
  {
    return ioc_;
  }

  boost::asio::io_context ioc_ {};
  boost::asio::executor_work_guard<decltype(ioc_.get_executor())> work {ioc_.get_executor()};
};

inline auto default_config()
{
  return mgmt::home_assistant::v2::ClientConfig{
    .unique_id = DefaultUniqueId,
    .username = DefaultUsername,
    .password = DefaultPassword,
    .host = DefaultMqttServerAddress,
    .port = DefaultMqttServerPort
  };
}

inline auto local_config()
{
  return mgmt::home_assistant::v2::ClientConfig{
    .unique_id = DefaultUniqueId,
    .username = LocalUsername,
    .password = LocalPassword,
    .host = LocalMqttServerAddress,
    .port = LocalMqttServerPort,
    .keep_alive = 0x1234
  };
}

inline auto config_from_options()
{
  auto unique_id = TestConfig::get()
    .option_value(MqttUniqueIdOptionName).value_or(DefaultUniqueId);
  auto username = TestConfig::get()
    .option_value(MqttUsernameOptionName).value_or(DefaultUsername);
  auto password = TestConfig::get()
    .option_value(MqttPasswordOptionName).value_or(DefaultPassword);
  auto server_address = TestConfig::get()
    .option_value(MqttServerAddressOptionName).value_or(DefaultMqttServerAddress);
  auto server_port = TestConfig::get()
    .option_value(DefaultMqttServerPort).value_or(DefaultMqttServerPort);

  return mgmt::home_assistant::v2::ClientConfig {
    .unique_id = unique_id,
    .username = username,
    .password = password,
    .host = server_address,
    .port = server_port,
    .clean_session = true,
    .keep_alive = DefaultKeepAlive
  };
}

inline auto get_config()
{
    return local_config();
}

inline auto setup_logger(const std::string& logger_name, spdlog::level::level_enum level)
{
    // Console sink
    auto sinks = std::vector<spdlog::sink_ptr>{};
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(level);
    sinks.push_back(console_sink);

    auto logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::trace);
    spdlog::register_logger(logger);
}
