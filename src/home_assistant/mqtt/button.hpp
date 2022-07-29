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
struct ButtonConfig
{
  static constexpr inline auto CommandTopicKey = std::string_view{ "command_topic" };
  static constexpr inline auto CommandTopicValue = "set";
  static constexpr inline auto TopicEntityName = "button";
  static constexpr inline auto PayloadPressKey = "payload_press";
  static constexpr inline auto PayloadPressValue = "press";
};

using ButtonCommandHandler_t = std::function<void(std::string_view)>;

template<class EntityClient>
class Button : public Entity<EntityClient>
{
  using Base_t = Entity<EntityClient>;
  using Base_t::unique_id_;
  using Base_t::client_;
  using Base_t::topic;
  using Base_t::async_set_availibility;

public:
  Button() = delete;
  Button(std::string uid, EntityClient client)// TODO: Consider passing EntityClient by rvalue ref
    : Base_t(std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}, unique_id: {}", __FUNCTION__, unique_id_);
  }

  Button(const Button&) = delete;
  Button& operator=(const Button&) = delete;
  Button(Button&& rhs) noexcept = default;
  Button& operator=(Button&&) noexcept = default;

  void on_command(ButtonCommandHandler_t handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("ButtonConfig::{}", __FUNCTION__);

    subs_[topics_.at(ButtonConfig::CommandTopicKey)] = [handler = std::move(handler)](auto&& content) {
      handler(content);
    };
  }

  void async_set_config(EntityConfig config)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}", __FUNCTION__);

    config.set_override(ButtonConfig::CommandTopicKey, topics_.at(ButtonConfig::CommandTopicKey));
    config.set_override(ButtonConfig::PayloadPressKey, ButtonConfig::PayloadPressValue);
    config.set_override(GenericEntityConfig::AvailibilityTopic, topics_.at(GenericEntityConfig::AvailibilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    client_.subscribe(subs_.begin(), subs_.end());
    client_.async_publish(fmt::format("homeassistant/button/{}/config", unique_id_), config.parse());
  }

  void async_set_availability(const Availability& availibility)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}", __FUNCTION__);

    async_set_availibility(topics_.at(GenericEntityConfig::AvailibilityTopic), availibility);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 4> topics_{
    std::pair{ ButtonConfig::CommandTopicKey, topic(ButtonConfig::TopicEntityName, ButtonConfig::CommandTopicValue) },
    std::pair{ GenericEntityConfig::AvailibilityTopic, topic(ButtonConfig::TopicEntityName, GenericEntityConfig::AvailibilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(ButtonConfig::TopicEntityName, GenericEntityConfig::JsonAttributesTopic) },
  };
  std::unordered_map<std::string, PublishHandler_t> subs_{
    std::pair{ topic(ButtonConfig::TopicEntityName, ButtonConfig::CommandTopicValue), default_publish_handler() }
  };
};
}// namespace mgmt::home_assistant::mqttc