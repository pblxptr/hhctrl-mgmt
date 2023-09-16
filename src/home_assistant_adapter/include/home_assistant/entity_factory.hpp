#pragma once

#include <spdlog/spdlog.h>

#include <home_assistant/mqtt/cover.hpp>
#include <home_assistant/mqtt/binary_sensor.hpp>
#include <home_assistant/mqtt/button.hpp>
#include <home_assistant/mqtt/sensor.hpp>
#include <home_assistant/client_factory.hpp>

namespace mgmt::home_assistant::adapter {
class EntityFactory
{
public:
  explicit EntityFactory(MqttClientFactory factory)
    : client_factory_{std::move(factory)}
  {}

  [[nodiscard]] auto create_cover(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqtt::Cover{unique_id, client_factory_.create_client(unique_id)};
  }

  [[nodiscard]] auto create_binary_sensor(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqtt::BinarySensor{unique_id, client_factory_.create_client(unique_id)};
  }

  [[nodiscard]] auto create_button(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqtt::Button(unique_id, client_factory_.create_client(unique_id));
  }

  [[nodiscard]] auto create_sensor(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqtt::Sensor(unique_id, client_factory_.create_client(unique_id));
  }

private:
    MqttClientFactory client_factory_;
};

using Cover_t = decltype(std::declval<EntityFactory>().create_cover(std::declval<std::string>()));
using BinarySensor_t = decltype(std::declval<EntityFactory>().create_binary_sensor(std::declval<std::string>()));
using Button_t = decltype(std::declval<EntityFactory>().create_button(std::declval<std::string>()));
using Sensor_t = decltype(std::declval<EntityFactory>().create_sensor(std::declval<std::string>()));

}// namespace mgmt::home_assistant
