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

struct SensorConfig
{
  static constexpr inline auto EntityName = "sensor";

  struct Default {
      static constexpr inline auto StateTopic = std::string_view{"state"};
  };

  struct Property {
      static constexpr inline auto StateTopic = std::string_view{ "state_topic" };
  };
};

using SensorState = std::string;

template<class EntityClient>
class Sensor : public Entity<EntityClient>
{
    using BaseType = Entity<EntityClient>;
    using BaseType::topic;
    using BaseType::async_publish;
public:
    using BaseType::unique_id;
    using BaseType::async_set_availability;

  Sensor() = delete;
  Sensor(std::string uid, std::unique_ptr<EntityClient> client)
    : BaseType(SensorConfig::EntityName, std::move(uid), std::move(client))
  {
    logger::trace(logger::Entity, "Sensor::{}, unique_id: {}", __FUNCTION__, unique_id());
  }
  // movable
  Sensor(Sensor&& rhs) noexcept = default;
  Sensor& operator=(Sensor&&) noexcept = default;
  // non-copyable
  Sensor(const Sensor&) = delete;
  Sensor& operator=(const Sensor&) = delete;

  ~Sensor() = default;

    boost::asio::awaitable<Error> async_configure(EntityConfig config = EntityConfig{}, Pubopts_t pubopts = DefaultPubOpts)
  {
    logger::trace(logger::Entity, "Sensor::{}", __FUNCTION__);

    config.set_override(SensorConfig::Property::StateTopic, topics_.at(SensorConfig::Property::StateTopic));

    config.set_override(GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

      // Set config
      if (const auto error = co_await BaseType::async_set_config(std::move(config), pubopts); error) {
          co_return error;
      }

      co_return Error{};
  }

  boost::asio::awaitable<Error> async_set_state(const SensorState& state, Pubopts_t pubopts = DefaultPubOpts)
  {
    logger::debug(logger::Entity, "SensorState::{}, state: {}", __FUNCTION__, state);

    co_return co_await BaseType::async_publish(topics_.at(SensorConfig::Property::StateTopic), state, pubopts);
  }

  boost::asio::awaitable<Error> async_receive()
  {
    const auto& packet = co_await BaseType::async_receive();

    if (!packet) {
        co_return packet.error();
    }
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 2> topics_{
    std::pair{ SensorConfig::Property::StateTopic, topic(SensorConfig::Default::StateTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc
