//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>
#include <home_assistant/mqtt/entity.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::mqtt {
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
    using BaseType::async_publish;
public:
    using BaseType::unique_id;
    using BaseType::async_set_availability;

  BinarySensor() = delete;
  BinarySensor(std::string uid, std::unique_ptr<EntityClient> client)
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

    config.set_override(GenericEntityConfig::AvailabilityTopic, BaseType::topic(GenericEntityConfig::AvailabilityTopic));
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

  boost::asio::awaitable<Error> async_receive()
  {
    const auto& packet = co_await BaseType::async_receive();

    if (!packet) {
        co_return packet.error();
    }

    co_return Error {};
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 2> topics_{
    std::pair{ BinarySensorConfig::Property::StateTopic, BaseType::topic(BinarySensorConfig::Default::StateTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, BaseType::topic(GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc
