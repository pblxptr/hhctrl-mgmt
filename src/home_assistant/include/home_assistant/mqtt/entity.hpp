//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

#include <home_assistant/availability.hpp>
#include <home_assistant/logger.hpp>
#include <utils/mapper.hpp>

namespace mgmt::home_assistant::mqttc {
namespace details {
  constexpr static auto AvailabilityStateMapper = common::utils::Mapper{
    std::pair{ Availability::Offline, "offline" },
    std::pair{ Availability::Online, "online" }
  };
} // namespace details

struct GenericEntityConfig
{
  static constexpr inline auto AvailabilityTopic = std::string_view{ "availability_topic" };
  static constexpr inline auto JsonAttributesTopic = std::string_view{ "json_attributes_topic" };
  static constexpr inline auto JsonAttributesTemplate = std::string_view{ "json_attributes_template" };
};

template<class EntityClient>
class Entity
{
public:
  Entity() = delete;

  // movable
  Entity(Entity&& rhs) noexcept = default;
  Entity& operator=(Entity&&) noexcept = default;
  // non-copyable
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;

  ~Entity() = default;

  const std::string& unique_id() const
  {
    return unique_id_;
  }

  boost::asio::awaitable<void> async_connect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    co_await client_.async_connect();
  }

  template<class Handler>
  void set_ack_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->trace("Entity::{}", __FUNCTION__);

    client_.set_connack_handler(std::move(handler));
  }

  template<class Handler>
  void set_error_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->trace("Entity::{}", __FUNCTION__);

    client_.set_error_handler(std::move(handler));
  }

protected:
  Entity(std::string_view entity_name, std::string unique_id, EntityClient client)
    : entity_name_{entity_name}
    , unique_id_{ std::move(unique_id) }
    , client_{ std::move(client) }
  {
    client_.set_will(Will {
        .topic = topic(GenericEntityConfig::AvailabilityTopic),
        .payload = details::AvailabilityStateMapper.map(Availability::Offline)
      }
    );
  }

  // TODO(pp): Consider passing topic by ref
  template<class Payload>
  boost::asio::awaitable<void> async_publish(std::string topic, const Payload& payload)
  {
    co_await client_.async_publish(topic, payload);
  }

  template<class Iterator>
  boost::asio::awaitable<void> async_subscribe(Iterator begin, Iterator end)
  {
    co_await client_.async_subscribe(begin, end);
  }

  boost::asio::awaitable<void> async_set_availability(const std::string& topic, const Availability& availability)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}, availability: {}", id(), __FUNCTION__,
      details::AvailabilityStateMapper.map(availability));

    //TODO(bielpa): Perhaps remove std::string {}
    co_await client_.async_publish(topic, std::string {details::AvailabilityStateMapper.map(availability)} );
  }

  template<class T1>
  std::string topic(const T1& topic) const
  {
    return fmt::format("{}_{}/{}", entity_name_, unique_id_, topic);
  }

private:
  std::string id() const
  {
    return fmt::format("{}-{}", entity_name_, unique_id_);
  }

private:
  std::string_view entity_name_;
  std::string unique_id_;
  EntityClient client_;
};
}// namespace mgmt::home_assistant::mqttc
