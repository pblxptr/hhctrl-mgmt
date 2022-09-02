//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <home_assistant/availability.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::mqttc {
struct GenericEntityConfig
{
  static constexpr inline auto availabilityTopic = std::string_view{ "availability_topic" };
  static constexpr inline auto JsonAttributesTopic = std::string_view{ "json_attributes_topic" };
  static constexpr inline auto JsonAttributesTemplate = std::string_view{ "json_attributes_template" };
};


template<class EntityClient>
class Entity
{
public:
  Entity() = delete;
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;
  Entity(Entity&& rhs) noexcept = default;
  Entity& operator=(Entity&&) noexcept = default;

  const std::string& unique_id() const
  {
    return unique_id_;
  }

  boost::asio::awaitable<void> async_connect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity::{}", __FUNCTION__);

    co_await client_.async_connect();
  }

  template<class Handler>
  void set_ack_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity::{}", __FUNCTION__);

    client_.set_connack_handler(std::move(handler));
  }

  template<class Handler>
  void set_error_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity::{}", __FUNCTION__);

    client_.set_error_handler(std::move(handler));
  }

protected:
  Entity(std::string unique_id, EntityClient client)
    : unique_id_{ std::move(unique_id) }
    , client_{ std::move(client) }
  {}

  boost::asio::awaitable<void> async_set_availability(const std::string& topic, const Availability& availability)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity::{}", __FUNCTION__);

    auto availability_str = std::string{};

    switch (availability) {
    case Availability::Offline:
      availability_str = "offline";
      break;
    case Availability::Online:
      availability_str = "online";
      break;
    }

    co_await client_.async_publish(topic, availability_str);
  }


  template<class T1, class T2>
  std::string topic(const T1& entity_name, const T2& topic_spec) const
  {
    return fmt::format("{}_{}/{}", entity_name, unique_id_, topic_spec);
  }

protected:
  std::string unique_id_;
  EntityClient client_;
};
}// namespace mgmt::home_assistant::mqttc