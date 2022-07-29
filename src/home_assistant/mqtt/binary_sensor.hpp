//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <common/utils/mapper.hpp>
#include <common/utils/static_map.hpp>
#include <home_assistant/mqtt/entity.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/mqtt/entity_client.hpp>

namespace mgmt::home_assistant::mqttc {
enum class BinarySensorState { Off,
  On };
struct BinarySensorConfig
{
  static constexpr inline auto StateTopicKey = std::string_view{ "state_topic" };
  static constexpr inline auto StateTopicValue = "state";
  static constexpr inline auto TopicEntityName = "binary_sensor";
  static constexpr inline auto StateOffKey = "payload_off";
  static constexpr inline auto StateOnKey = "payload_on";
};

constexpr static auto BinarySensorStateMapper = common::utils::Mapper{
  std::pair{ BinarySensorState::Off, "off" },
  std::pair{ BinarySensorState::On, "on" }
};

template<class EntityClient>
class BinarySensor : public Entity<EntityClient>
{
  using Base_t = Entity<EntityClient>;
  using Base_t::unique_id_;
  using Base_t::client_;
  using Base_t::topic;
  using Base_t::async_set_availibility;

public:
  BinarySensor() = delete;
  BinarySensor(std::string uid, EntityClient client)// TODO: Consider passing EntityClient by rvalue ref
    : Base_t(std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("BinarySensor::{}, unique_id: {}", __FUNCTION__, unique_id_);
  }

  BinarySensor(const BinarySensor&) = delete;
  BinarySensor& operator=(const BinarySensor&) = delete;
  BinarySensor(BinarySensor&& rhs) noexcept = default;
  BinarySensor& operator=(BinarySensor&&) noexcept = default;

  void async_set_config(EntityConfig config)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("BinarySensor::{}", __FUNCTION__);

    config.set_override(BinarySensorConfig::StateTopicKey, topics_.at(BinarySensorConfig::StateTopicKey));
    config.set_override(BinarySensorConfig::StateOffKey, std::string{ BinarySensorStateMapper.map(BinarySensorState::Off) });
    config.set_override(BinarySensorConfig::StateOnKey, std::string{ BinarySensorStateMapper.map(BinarySensorState::On) });

    config.set_override(GenericEntityConfig::AvailibilityTopic, topics_.at(GenericEntityConfig::AvailibilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    client_.async_publish(fmt::format("homeassistant/binary_sensor/{}/config", unique_id_), config.parse());
  }

  void async_set_state(const BinarySensorState& state)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("BinarySensorState::{}", __FUNCTION__);

    client_.async_publish(topics_.at(BinarySensorConfig::StateTopicKey), std::string{ BinarySensorStateMapper.map(state) });
  }

  void async_set_availability(const Availability& availibility)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("BinarySensorState::{}", __FUNCTION__);

    async_set_availibility(topics_.at(GenericEntityConfig::AvailibilityTopic), availibility);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 3> topics_{
    std::pair{ BinarySensorConfig::StateTopicKey, topic(BinarySensorConfig::TopicEntityName, BinarySensorConfig::StateTopicValue) },
    std::pair{ GenericEntityConfig::AvailibilityTopic, topic(BinarySensorConfig::TopicEntityName, GenericEntityConfig::AvailibilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(BinarySensorConfig::TopicEntityName, GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc