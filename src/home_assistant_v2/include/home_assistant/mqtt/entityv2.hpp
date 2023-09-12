//
// Created by bielpa on 24.08.23.
//

#pragma once

//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>
#include <unordered_set>

#include <home_assistant/mqtt/availability2.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <home_assistant/mqtt/entity_configv2.hpp>
#include <utils/mapper.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::v2 {

namespace detail {
  constexpr static auto AvailabilityStateMapper = common::utils::Mapper{
    std::pair{ Availability::Offline, "offline" },
    std::pair{ Availability::Online, "online" }
  };

} // namespace detail

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

  boost::asio::awaitable<Error> async_connect()
  {
    logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

    co_return co_await client_->async_connect();
  }

protected:
  Entity(std::string_view entity_name, std::string unique_id, std::unique_ptr<EntityClient> client)
    : entity_name_{ entity_name }
    , unique_id_{ std::move(unique_id) }
    , client_{ std::move(client) }
  {
    client_->set_will(WillConfig {
      .topic = topic(GenericEntityConfig::AvailabilityTopic),
      .message = std::string{detail::AvailabilityStateMapper.map(Availability::Offline)},
      .pubopts = Retain_t::yes //TODO(bielpa): Consider removing will default configuration from
    }
    );
  }

  void set_will(const WillConfig& will)
  {
    client_->set_will(will);
  }

  boost::asio::awaitable<Expected<PublishPacket_t>> async_receive()
  {
    logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

    while (true) {
      const auto& packet = co_await client_->async_receive();

      if (!packet) {
        logger::debug(logger::Entity, "Entity: {}, error: {}", full_id(), packet.error().what());
        co_return Unexpected { packet.error() };
      }

      const auto& value = packet.value();

      if (std::holds_alternative<PublishPacket_t>(value)) {
        co_return Expected<PublishPacket_t>{std::get<PublishPacket_t>(value)};
      }
      else if (std::holds_alternative<SubscriptionAckPacket_t>(value)) {
        const auto& suback_packet = std::get<SubscriptionAckPacket_t>(value);

        if (any_suback_failure(suback_packet)) {
          co_return Unexpected{ErrorCode::SubscriptionFailure};
        }
      }
      else if (std::holds_alternative<PublishAckPacket_t>(value)) {
        const auto& puback_packet = std::get<PublishAckPacket_t>(value);
        if (pending_puback_.contains(puback_packet.packet_id())) {
          pending_puback_.erase(puback_packet.packet_id());
        }
      }
    }
  }

  boost::asio::awaitable<Error> async_set_config(EntityConfig config, Pubopts_t pubopts)
  {
    logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

    config.set_override("unique_id", unique_id_);

    {
      const auto error = co_await async_publish(fmt::format("homeassistant/{}/{}/config", entity_name_, unique_id()), config.parse(), pubopts);

      if (error) {
        logger::err(logger::Entity, "Entity {}, error: {}", full_id(), error.what());

        co_return Error { ErrorCode::InvalidConfig, error.what() };
      }
    }

    {
      if (pubopts.get_qos() > Qos_t::at_most_once) {
        auto packet = co_await client_->async_receive();
        if (!packet) {
          logger::err(logger::Entity, "Entity {}, error: {}", full_id(), packet.error().what());

          co_return Error { ErrorCode::InvalidConfig, packet.error().what() };
        }
        const auto& value = packet.value();
        if (!std::holds_alternative<PublishAckPacket_t >(value)) {
          auto err = Error { ErrorCode::InvalidConfig, "Expected PublishAckPacket_t was not received" };
          logger::err(logger::Entity, "Entity , {}, error: {}", full_id(), err.what());

          co_return err;
        }
      }
    }

    co_return std::error_code{};
  }

  boost::asio::awaitable<Error> async_set_availability(Availability availability, Pubopts_t pubopts)
  {
      logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

      co_return co_await async_publish(topic(GenericEntityConfig::AvailabilityTopic), detail::AvailabilityStateMapper.map(availability), pubopts);
  }

  template<typename Topic, class Payload>
  boost::asio::awaitable<Error> async_publish(Topic&& topic, Payload&& payload, Pubopts_t pubopts)
  {
    logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

    const auto result = co_await client_->async_publish(std::forward<Topic>(topic), std::forward<Payload>(payload), pubopts);

    if (!result) {
      co_return result.error();
    }

    if (pubopts.get_qos() > Qos_t::at_most_once) {
      pending_puback_.insert(result.value());
    }

    co_return std::error_code{};
  }

  boost::asio::awaitable<Error> async_subscribe(std::vector<std::string> sub_topics)
  {
    logger::trace(logger::Entity, "Entity::{}, {}", __FUNCTION__, full_id());

    {
      const auto result = co_await client_->async_subscribe(sub_topics);

      if (!result) {
        logger::err(logger::Entity, "Entity {}, error: {}", full_id(), result.error().what());

        co_return Error { ErrorCode::SubscriptionFailure, result.error().what() };
      }
    }

    {
      auto packet = co_await client_->async_receive();
      if (!packet) {
        logger::err(logger::Entity, "Entity {}, error: {}", full_id(), packet.error().what());

        co_return Error { ErrorCode::SubscriptionFailure, packet.error().what() };
      }

      const auto& value = packet.value();
      if (!std::holds_alternative<SubscriptionAckPacket_t>(value)) {
        auto err = Error { ErrorCode::SubscriptionFailure, "Expected SubscriptionAckPacket_t was not received" };
        logger::err(logger::Entity, "Entity {}, error: {}", full_id(), err.what());

        co_return err;
      }
      const auto& suback_packet = std::get<SubscriptionAckPacket_t>(value);
      if (any_suback_failure(suback_packet)) {
        co_return Error { ErrorCode::SubscriptionFailure, "One of the subscribed topic has not received the required subscription acknowledge" };
      }

      logger::debug(logger::Entity, "Entity {} subscription confirmed", full_id());
    }

    co_return Error{};
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

  std::string full_id() const
  {
    return fmt::format("name: '{}', unique_id: '{}'", entity_name_, unique_id_);
  }

private:
  std::string_view entity_name_;
  std::string unique_id_;
  std::unique_ptr<EntityClient> client_;
  std::unordered_set<PacketId_t> pending_puback_;
};
}// namespace mgmt::home_assistant::v2
