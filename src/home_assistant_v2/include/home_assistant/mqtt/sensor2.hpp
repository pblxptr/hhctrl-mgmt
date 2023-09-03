//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>
#include <home_assistant/mqtt/entityv2.hpp>

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

template<class EntityClient>
class Sensor : public Entity<EntityClient>
{
    using BaseType = Entity<EntityClient>;
    using BaseType::topic;
    using BaseType::async_publish;
public:
    using BaseType::unique_id;


  Sensor() = delete;
  Sensor(std::string uid, EntityClient client)// TODO(pp): Consider passing EntityClient by rvalue ref
    : BaseType(SensorConfig::EntityName, std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->trace("Sensor::{}, unique_id: {}", __FUNCTION__, unique_id());
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
    common::logger::get(mgmt::home_assistant::Logger)->trace("Sensor::{}", __FUNCTION__);

    config.set_override(SensorConfig::Property::StateTopic, topics_.at(SensorConfig::Property::StateTopic));

    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

      // Set config
      if (const auto error = co_await BaseType::async_set_config(std::move(config), pubopts); error) {
          co_return error;
      }

      co_return Error{};
  }

    boost::asio::awaitable<Error> async_set_state(const std::string& state, Pubopts_t pubopts = DefaultPubOpts)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("SensorState::{}, state: {}", __FUNCTION__, state);

    co_return co_await BaseType::async_publish(topics_.at(SensorConfig::Property::StateTopic), state, pubopts);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 3> topics_{
    std::pair{ SensorConfig::Property::StateTopic, topic(SensorConfig::Default::StateTopic) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(GenericEntityConfig::JsonAttributesTopic) },
  };
};
}// namespace mgmt::home_assistant::mqttc
