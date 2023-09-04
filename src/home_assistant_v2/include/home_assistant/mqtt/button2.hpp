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
struct ButtonConfig
{
  static constexpr inline auto EntityName = std::string_view{"button"};

  struct Default {
      static constexpr inline auto CommandTopic = std::string_view{"set"};
      static constexpr inline auto PayloadPress = std::string_view{"press"};
  };

  struct Property {
      static constexpr inline auto CommandTopic = std::string_view{"command_topic"};
      static constexpr inline auto PayloadPress = std::string_view{"payload_press"};
  };
};

struct PressButtonCommand {};

using ButtonCommand = PressButtonCommand;

template<class EntityClient>
class Button : public Entity<EntityClient>
{
    using BaseType = Entity<EntityClient>;
    using BaseType::topic;
    using BaseType::async_publish;

public:
    using BaseType::unique_id;
    using BaseType::async_receive;

  Button() = delete;
  Button(std::string uid, EntityClient client)
    : BaseType(ButtonConfig::EntityName, std::move(uid), std::move(client))
  {
    logger::trace(logger::Entity, "Button::{}, unique_id: {}", __FUNCTION__, unique_id());
  }
  // movable
  Button(Button&& rhs) noexcept = default;
  Button& operator=(Button&&) noexcept = default;
  // non-copyable
  Button(const Button&) = delete;
  Button& operator=(const Button&) = delete;

  ~Button() = default;


  boost::asio::awaitable<Error> async_configure(EntityConfig config = EntityConfig{}, Pubopts_t pubopts = DefaultPubOpts)
  {
    logger::trace(logger::Entity, "Button::{}", __FUNCTION__);

    config.set_override(ButtonConfig::Property::CommandTopic, topics_.at(ButtonConfig::Property::CommandTopic));
    config.set_override(ButtonConfig::Property::PayloadPress, ButtonConfig::Default::PayloadPress);
    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

      // Set config
      if (const auto error = co_await BaseType::async_set_config(std::move(config), pubopts); error) {
          co_return error;
      }

      // Set subscriptions
      auto sub_topics = std::vector<std::string>{topics_.at(ButtonConfig::Property::CommandTopic)};

      if (const auto error = co_await BaseType::async_subscribe(std::move(sub_topics)); error) {
          co_return error;
      }

      co_return Error{};
  }

  boost::asio::awaitable<Expected<ButtonCommand>> async_receive()
  {
      const auto& packet = co_await BaseType::async_receive();

      if (!packet) {
          co_return Unexpected{ packet.error() };
      }

      using std::to_string;

      const auto& payload = to_string(packet.value().payload());

      if (payload == ButtonConfig::Default::PayloadPress) {
          co_return PressButtonCommand{};
      }

      co_return Unexpected { ErrorCode::UnknownPacket };
    }

private:
  common::utils::StaticMap<std::string_view, std::string, 4> topics_{
    std::pair{ ButtonConfig::Property::CommandTopic, topic(ButtonConfig::Default::CommandTopic) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic) }
  };
};
}// namespace mgmt::home_assistant::v2
