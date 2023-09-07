#pragma once

#include <spdlog/spdlog.h>

#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <home_assistant/mqtt/cover.hpp>
#include <home_assistant/mqtt/binary_sensor.hpp>
#include <home_assistant/mqtt/button.hpp>
#include <home_assistant/mqtt/sensor.hpp>

namespace mgmt::home_assistant {
class EntityFactory
{
public:
  explicit EntityFactory(const mgmt::home_assistant::mqttc::EntityClientFactory& client_factory)
    : client_factory_{ client_factory }
  {}

  [[nodiscard]] auto create_cover(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqttc::Cover(unique_id, client_factory_.create_async_client(unique_id));
  }

  [[nodiscard]] auto create_binary_sensor(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqttc::BinarySensor(unique_id, client_factory_.create_async_client(unique_id));
  }

  [[nodiscard]] auto create_button(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqttc::Button(unique_id, client_factory_.create_async_client(unique_id));
  }

  [[nodiscard]] auto create_sensor(const std::string& unique_id) const
  {
    return mgmt::home_assistant::mqttc::Sensor(unique_id, client_factory_.create_async_client(unique_id));
  }

private:
  const mgmt::home_assistant::mqttc::EntityClientFactory& client_factory_;
};
}// namespace mgmt::home_assistant
