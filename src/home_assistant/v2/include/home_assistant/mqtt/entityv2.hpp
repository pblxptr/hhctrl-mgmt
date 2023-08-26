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

#include <home_assistant/availability.hpp>
#include <home_assistant/mqtt/will.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/mqtt/entity_configv2.hpp>
#include <utils/mapper.hpp>
#include <home_assistant/mqtt/entity_errorv2.hpp>
#include <home_assistant/logger.hpp>

//TODO(bielpa) Use global qos per client.

namespace mgmt::home_assistant::v2 {

using PublishHandler = std::function<void(std::string)>;


using Availability = mgmt::home_assistant::mqttc::Availability;

namespace detail {
  bool any_suback_failure(const SubscriptionAckPacket& suback_packet)
  {
    return std::ranges::any_of(suback_packet.entries(), [](const auto& suback_code) {
      return suback_code == async_mqtt::suback_return_code::failure;
    });
  }

} // namespace detail

namespace details { //TODO(bielpa): MOve details to detail
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

//using EntityReceiveExpected = tl::expected<std::variant<PublishPacket, SubscriptionAckPacket>, std::error_code>;
//using EntityReceiveUnexpected = tl::unexpected<std::error_code>;

template<class EntityClient>
class Entity
{
  using Will = mgmt::home_assistant::mqttc::Will;
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

  boost::asio::awaitable<std::error_code> async_connect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    co_return co_await client_.async_connect();
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

  boost::asio::awaitable<Expected<PublishPacket>> async_receive()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    while (true) {
      const auto& packet = co_await client_.async_receive();

      if (!packet) {
        common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}, packet error", id(), __FUNCTION__);
        co_return Unexpected { packet.error() };
      }

      const auto& value = packet.value();

      if (std::holds_alternative<PublishPacket>(value)) {
        co_return Expected<PublishPacket>{std::get<PublishPacket>(value)};
      }
      else if (std::holds_alternative<SubscriptionAckPacket>(value)) {
        const auto& suback_packet = std::get<SubscriptionAckPacket>(value);

        if (detail::any_suback_failure(suback_packet)) {
          co_return Unexpected{EntityError::SubscriptionError};
        }
      }
      else if (std::holds_alternative<PublishAckPacket>(value)) {
        const auto& puback_packet = std::get<PublishAckPacket>(value);
        if (pending_puback_.contains(puback_packet.packet_id())) {
          pending_puback_.erase(puback_packet.packet_id());
        }
      }
    }
  }

  boost::asio::awaitable<std::error_code> async_set_config(EntityConfig config, QOS qos)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    config.set_override("unique_id", unique_id_);

    {
      const auto error_code = co_await async_publish(fmt::format("homeassistant/{}/{}/config", entity_name_, unique_id()), config.parse());

      if (error_code) {
        common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: {}", __FUNCTION__, unique_id(), error_code.message());

        co_return EntityError::ConfigError;
      }
    }

    {
      if (qos > QOS::at_most_once) {
        auto packet = co_await client_.async_receive();
        if (!packet) {
          common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: {}", __FUNCTION__, unique_id(), packet.error().message());
          co_return EntityError::ConfigError;
        }
        const auto& value = packet.value();
        if (!std::holds_alternative<PublishAckPacket >(value)) {
          common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: Expected PublishAckPacket was not received.", __FUNCTION__, unique_id());
          co_return EntityError::ConfigError;
        }
      }
    }

    co_return std::error_code{};
  }

  template<typename Topic, class Payload>
  boost::asio::awaitable<std::error_code> async_publish(Topic&& topic, Payload&& payload, QOS qos = DefaultQoS)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    const auto result = co_await client_.async_publish(std::forward<Topic>(topic), std::forward<Payload>(payload), qos);

    if (!result) {
      co_return result.error();
    }

    if (qos > QOS::at_most_once) {
      pending_puback_.insert(result.value());
    }

    co_return std::error_code{};
  }

  boost::asio::awaitable<std::error_code> async_subscribe(std::vector<std::string> sub_topics)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}", id(), __FUNCTION__);

    {
      const auto result = co_await client_.async_subscribe(sub_topics);

      if (!result) {
        common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: {}", __FUNCTION__, unique_id(), result.error().message());
        co_return EntityError::SubscriptionError;
      }
    }

    {
      auto packet = co_await client_.async_receive();
      if (!packet) {
        common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: {}", __FUNCTION__, unique_id(), packet.error().message());
        co_return EntityError::SubscriptionError;
      }

      const auto& value = packet.value();
      if (!std::holds_alternative<SubscriptionAckPacket>(value)) {
        common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, error: Expected SubscriptionAckPacket was not received.", __FUNCTION__, unique_id());
        co_return EntityError::SubscriptionError;
      }
      const auto& suback_packet = std::get<SubscriptionAckPacket>(value);
      if (detail::any_suback_failure(suback_packet)) {
        for (std::size_t idx = 0; idx < suback_packet.entries().size(); idx++) {
          if (suback_packet.entries()[idx] == SubscriptionAckReturnCode::failure) {
            common::logger::get(mgmt::home_assistant::Logger)->error("Entity::{}, unique_id: {}, sub topic: {} failure",
              __FUNCTION__, unique_id(), sub_topics[idx]);
          }
        }
        co_return EntityError::SubscriptionError;
      }
      common::logger::get(mgmt::home_assistant::Logger)->debug("Entity::{}, unique_id: {}, subscription confirmed.", __FUNCTION__, unique_id());
    }

    co_return std::error_code{};
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
  std::unordered_set<PacketId> pending_puback_;
};
}// namespace mgmt::home_assistant::v2
