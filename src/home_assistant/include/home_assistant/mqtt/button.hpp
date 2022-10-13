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
struct ButtonConfig
{
  static constexpr inline auto EntityName = "button";
  static constexpr inline auto CommandTopicKey = std::string_view{ "command_topic" };
  static constexpr inline auto CommandTopicValue = "set";
  static constexpr inline auto PayloadPressKey = "payload_press";
  static constexpr inline auto PayloadPressValue = "press";
};

using ButtonCommandHandler_t = std::function<void()>;

template<class EntityClient>
class Button : public Entity<EntityClient>
{
  using Base_t = Entity<EntityClient>;
  using Base_t::topic;
  using Base_t::async_set_availability;
  using Base_t::async_publish;
  using Base_t::async_subscribe;

public:
  using Base_t::unique_id;

  Button() = delete;
  Button(std::string uid, EntityClient client)// TODO(pp): Consider passing EntityClient by rvalue ref
    : Base_t(ButtonConfig::EntityName, std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}, unique_id: {}", __FUNCTION__, unique_id());
  }
  // movable
  Button(Button&& rhs) noexcept = default;
  Button& operator=(Button&&) noexcept = default;
  // non-copyable
  Button(const Button&) = delete;
  Button& operator=(const Button&) = delete;

  ~Button() = default;

  void on_command(ButtonCommandHandler_t handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("ButtonConfig::{}", __FUNCTION__);

    subs_[topics_.at(ButtonConfig::CommandTopicKey)] = [handler = std::move(handler)](auto&& /* content */) {
      handler();
    };
  }

  boost::asio::awaitable<void> async_set_config(EntityConfig config)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}", __FUNCTION__);

    config.set_override(ButtonConfig::CommandTopicKey, topics_.at(ButtonConfig::CommandTopicKey));
    config.set_override(ButtonConfig::PayloadPressKey, ButtonConfig::PayloadPressValue);
    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    co_await async_subscribe(subs_.begin(), subs_.end());
    co_await async_publish(fmt::format("homeassistant/button/{}/config", unique_id()), config.parse());
  }

  boost::asio::awaitable<void> async_set_availability(const Availability& availability)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Button::{}", __FUNCTION__);

    co_await async_set_availability(topics_.at(GenericEntityConfig::AvailabilityTopic), availability);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 4> topics_{
    std::pair{ ButtonConfig::CommandTopicKey, topic(ButtonConfig::CommandTopicValue) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic) }
  };
  std::unordered_map<std::string, PublishHandler_t> subs_{
    std::pair{ topic(ButtonConfig::CommandTopicValue), default_publish_handler() }
  };
};
}// namespace mgmt::home_assistant::mqttc
