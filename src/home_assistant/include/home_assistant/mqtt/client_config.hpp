//
// Created by pp on 7/29/22.
//

#pragma once

#include <string>
#include <chrono>
#include <boost/json.hpp>
#include <fmt/format.h>

namespace mgmt::home_assistant::mqttc {
struct EntityClientConfig
{
  std::string username { "user" };
  std::string password { "password" };
  std::string server_address{ "localhost" };
  int server_port{ 0 };
  int keep_alive_interval{ 30 };
  int max_reconnect_attempts{ 5 };
  std::chrono::seconds reconnect_delay{ std::chrono::seconds(5) };
};

inline std::string to_string(const EntityClientConfig& config)
{
  using std::to_string;

  return fmt::format(
    "server_address: {}\n"
    "server_port: {}\n"
    "keep_alive_interval: {}\n"
    "max_reconnect_attempts: {}\n"
    "reconnect_delay(seconds): {}\n",
    config.server_address,
    config.server_port,
    config.keep_alive_interval,
    config.max_reconnect_attempts,
    config.reconnect_delay.count());
}

inline mgmt::home_assistant::mqttc::EntityClientConfig tag_invoke(
  boost::json::value_to_tag<mgmt::home_assistant::mqttc::EntityClientConfig> /* unused */,
  const boost::json::value& jvalue)
{
  const auto& obj = jvalue.as_object();
  return mgmt::home_assistant::mqttc::EntityClientConfig{
    .username = boost::json::value_to<std::string>(obj.at("username")),
    .password = boost::json::value_to<std::string>(obj.at("password")),
    .server_address = boost::json::value_to<std::string>(obj.at("server_address")),
    .server_port = boost::json::value_to<int>(obj.at("server_port")),
    .keep_alive_interval = boost::json::value_to<int>(obj.at("keep_alive_interval")),
    .max_reconnect_attempts = boost::json::value_to<int>(obj.at("max_reconnect_attempts")),
    .reconnect_delay = std::chrono::seconds{ boost::json::value_to<int>(obj.at("reconnect_delay_seconds")) }
  };
}
}// namespace mgmt::home_assistant::mqttc
