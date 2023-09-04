//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>
#include <home_assistant/mqtt/entityv2.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::v2 {
enum class BinarySensorState { Off,
  On
};

struct BinarySensorConfig
{
  static constexpr inline auto EntityName = "binary_sensor";

  struct Default {
      static constexpr inline auto StateTopic = std::string_view{"state"};
  };

  struct Property {
      static constexpr inline auto StateTopic = std::string_view{ "state_topic" };
      static constexpr inline auto StateOff = std::string_view{ "payload_off" };
      static constexpr inline auto StateOn = std::string_view { "payload_on" };
  };
};

constexpr static auto BinarySensorStateMapper = common::utils::Mapper{
  std::pair{ BinarySensorState::Off, "off" },
  std::pair{ BinarySensorState::On, "on" }
};

template<class EntityClient>
class BinarySensor : public Entity<EntityClient>
{
    using BaseType = Entity<EntityClient>;
    using BaseType::topic;
    using BaseType::async_publish;
public:
    using BaseType::unique_id;

  BinarySensor() = delete;
  BinarySensor(std::string uid, EntityClient client)
    : BaseType(BinarySensorConfig::EntityName, std::move(uid), std::move(client))
  {
    logger::trace(logger::Entity, "BinarySensor::{}, unique_id: {}", __FUNCTION__, unique_id());
  }
  // movable
  BinarySensor(BinarySensor&& rhs) noexcept = default;
  BinarySensor& operator=(BinarySensor&&) noexcept = default;
  // non-copyable
  BinarySensor(const BinarySensor&) = delete;
  BinarySensor& operator=(const BinarySensor&) = delete;

  ~BinarySensor() = default;

  boost::asio::awaitable<Error> async_configure(EntityConfig config = EntityConfig{}, Pubopts_t pubopts = DefaultPubOpts)
  {
    logger::trace(logger::Entity, "BinarySensor::{}", __FUNCTION__);

    config.set_override(BinarySensorConfig::Property::StateTopic, topics_.at(BinarySensorConfig::Property::StateTopic));
    config.set_override(BinarySensorConfig::Property::StateOff, std::string{ BinarySensorStateMapper.map(BinarySensorState::Off) });
    config.set_override(BinarySensorConfig::Property::StateOn, std::string{ BinarySensorStateMapper.map(BinarySensorState::On) });

    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

      // Set config
      if (const auto error = co_await BaseType::async_set_config(std::move(config), pubopts); error) {
          co_return error;
      }

      co_return Error{};
  }

  boost::asio::awaitable<Error> async_set_state(const BinarySensorState& state, Pubopts_t pubopts = DefaultPubOpts)
  {

    logger::debug(logger::Entity, "BinarySensorState::{}, state: {}", __FUNCTION__, BinarySensorStateMapper.map(state));

    co_return co_await BaseType::async_publish(topics_.at(BinarySensorConfig::Property::StateTopic), std::string{ BinarySensorStateMapper.map(state) }, pubopts);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 3> topics_{
    std::pair{ BinarySensorConfig::Property::StateTopic, topic(BinarySensorConfig::Default::StateTopic) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc
