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

//TODO(bielpa) Use global qos per client.

namespace mgmt::home_assistant::v2 {
using PublishHandler = std::function<void(std::string)>;

inline auto DefaultQoS = QOS::at_least_once;

using Availability = mgmt::home_assistant::mqttc::Availability;

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

  boost::asio::awaitable<Expected<PublishPacket>> async_receive()
  {
    const auto packet = co_await client_.async_receive();

    if (!packet) {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}, packet error", id(), __FUNCTION__);
      co_return Unexpected{packet.error()};
    }
    auto pub_packet = std::visit(
      async_mqtt::overload {
      [&](const PublishPacket& pub_packet) -> std::optional<PublishPacket> {
        return pub_packet;
      },
      [&](const PublishAckPacket& puback_packet) -> std::optional<PublishPacket> {
          if (pending_puback_.contains(puback_packet.packet_id())) {
            pending_puback_.erase(puback_packet.packet_id());
            std::cout << "Confirm ack\n";
          }

          return std::nullopt;
      },
      [](const SubscriptionAckPacket& suback_packet) -> std::optional<PublishPacket> {
        return std::nullopt;
      }},
    packet.value());

    if (!pub_packet) {
      std::cout << "No pub..., receive again\n";
      co_await async_receive();
    }

    co_return Expected<PublishPacket>{std::move(*pub_packet)};
  }

  boost::asio::awaitable<std::error_code> async_set_config(EntityConfig config)
  {
    config.set_override("unique_id", unique_id_);

    co_return co_await async_publish(fmt::format("homeassistant/{}/{}/config", entity_name_, unique_id()), config.parse());
  }

  template<typename Topic, class Payload>
  boost::asio::awaitable<std::error_code> async_publish(Topic&& topic, Payload&& payload, QOS qos = QOS::at_least_once)
  {
    const auto result = co_await client_.async_publish(std::forward<Topic>(topic), std::forward<Payload>(payload));

    if (!result) {
      co_return result.error();
    }

    if (qos == QOS::at_least_once || qos == QOS::exactly_once) {
      pending_puback_.insert(result.value());
    }

    co_return std::error_code{};
  }

//  template<class Iterator>
//  boost::asio::awaitable<void> async_subscribe(Iterator begin, Iterator end)
//  {
//    co_await client_.async_subscribe(begin, end);
//  }

//  boost::asio::awaitable<void> async_set_availability(const std::string& topic, const Availability& availability)
//  {
//    common::logger::get(mgmt::home_assistant::Logger)->debug("Entity({})::{}, availability: {}", id(), __FUNCTION__,
//      details::AvailabilityStateMapper.map(availability));
//
//    //TODO(bielpa): Perhaps remove std::string {}
//    co_await client_.async_publish(topic, std::string {details::AvailabilityStateMapper.map(availability)} );
//  }

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
