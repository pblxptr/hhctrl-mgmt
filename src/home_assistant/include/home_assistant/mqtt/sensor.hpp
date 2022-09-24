//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>
#include <home_assistant/mqtt/entity.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/mqtt/entity_client.hpp>

namespace mgmt::home_assistant::mqttc {
struct SensorConfig
{
  static constexpr inline auto StateTopicKey = std::string_view{ "state_topic" };
  static constexpr inline auto StateTopicValue = "state";
  static constexpr inline auto TopicEntityName = "sensor";
};

template<class EntityClient>
class Sensor : public Entity<EntityClient>
{
  using Base_t = Entity<EntityClient>;
  using Base_t::topic;
  using Base_t::async_set_availability;
  using Base_t::async_publish;
public:
  using Base_t::unique_id;

  Sensor() = delete;
  Sensor(std::string uid, EntityClient client)// TODO(pp): Consider passing EntityClient by rvalue ref
    : Base_t(std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Sensor::{}, unique_id: {}", __FUNCTION__, unique_id());
  }
  // movable
  Sensor(Sensor&& rhs) noexcept = default;
  Sensor& operator=(Sensor&&) noexcept = default;

  // non-copyable
  Sensor(const Sensor&) = delete;
  Sensor& operator=(const Sensor&) = delete;

  ~Sensor() = default;

  boost::asio::awaitable<void> async_set_config(EntityConfig config)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Sensor::{}", __FUNCTION__);

    config.set_override(SensorConfig::StateTopicKey, topics_.at(SensorConfig::StateTopicKey));

    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    co_await async_publish(fmt::format("homeassistant/sensor/{}/config", unique_id()), config.parse());
  }

  boost::asio::awaitable<void> async_set_value(const std::string& value)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Sensor::{}", __FUNCTION__);

    co_await async_publish(topics_.at(SensorConfig::StateTopicKey), value);
  }

  boost::asio::awaitable<void> async_set_availability(const Availability& availability)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Sensor::{}", __FUNCTION__);

    co_await async_set_availability(topics_.at(GenericEntityConfig::AvailabilityTopic), availability);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 3> topics_{
    std::pair{ SensorConfig::StateTopicKey, topic(SensorConfig::TopicEntityName, SensorConfig::StateTopicValue) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(SensorConfig::TopicEntityName, GenericEntityConfig::AvailabilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(SensorConfig::TopicEntityName, GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc
