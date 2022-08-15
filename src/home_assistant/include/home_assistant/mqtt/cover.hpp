#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>

#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/mqtt/entity.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/availability.hpp>

namespace mgmt::home_assistant::mqttc {
enum class CoverState { Open,
  Opening,
  Closing,
  Closed };
enum class CoverCommand { Open,
  Close,
  Stop };

struct CoverConfig
{
  static constexpr inline auto StateTopicKey = std::string_view{ "state_topic" };
  static constexpr inline auto CommandTopicKey = std::string_view{ "command_topic" };
  static constexpr inline auto TopicEntityName = "cover";
  static constexpr inline auto StateTopicValue = "state";
  static constexpr inline auto CommandTopicValue = "set";
  static constexpr inline auto StateOpeningKey = "state_opening";
  static constexpr inline auto StateOpenKey = "state_open";
  static constexpr inline auto StateClosingKey = "state_closing";
  static constexpr inline auto StateClosedKey = "state_closed";
  static constexpr inline auto PayloadOpenKey = "payload_open";
  static constexpr inline auto PayloadCloseKey = "payload_close";
  static constexpr inline auto PayloadStopKey = "payload_stop";
};

constexpr static auto CoverStateMapper = common::utils::Mapper{
  std::pair{ CoverState::Open, "open" },
  std::pair{ CoverState::Opening, "opening" },
  std::pair{ CoverState::Closing, "closing" },
  std::pair{ CoverState::Closed, "closed" }
};

constexpr static auto CoverCommandMapper = common::utils::Mapper{
  std::pair{ CoverCommand::Open, "open" },
  std::pair{ CoverCommand::Close, "close" },
  std::pair{ CoverCommand::Stop, "stop" }
};
using CoverCommandHandler_t = std::function<void(const CoverCommand&)>;

template<class EntityClient>
class Cover : public Entity<EntityClient>
{
  using Base_t = Entity<EntityClient>;
  using Base_t::unique_id_;
  using Base_t::client_;
  using Base_t::topic;
  using Base_t::async_set_availibility;

public:
  Cover() = delete;
  Cover(std::string uid, EntityClient client)// TODO: Consider passing EntityClient by rvalue ref
    : Base_t(std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}, unique_id: {}", __FUNCTION__, unique_id_);
  }

  Cover(const Cover&) = delete;
  Cover& operator=(const Cover&) = delete;
  Cover(Cover&& rhs) noexcept = default;
  Cover& operator=(Cover&&) noexcept = default;

  void on_command(CoverCommandHandler_t handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

    subs_[topics_.at(CoverConfig::CommandTopicKey)] = [handler = std::move(handler)](auto&& content) {
      handler(CoverCommandMapper.map(content));
    };
  }

  void async_set_config(EntityConfig config)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

    config.set_override(CoverConfig::StateTopicKey, topics_.at(CoverConfig::StateTopicKey));
    config.set_override(CoverConfig::CommandTopicKey, topics_.at(CoverConfig::CommandTopicKey));
    config.set_override(CoverConfig::StateOpeningKey, std::string{ CoverStateMapper.map(CoverState::Opening) });
    config.set_override(CoverConfig::StateOpenKey, std::string{ CoverStateMapper.map(CoverState::Open) });
    config.set_override(CoverConfig::StateClosingKey, std::string{ CoverStateMapper.map(CoverState::Closing) });
    config.set_override(CoverConfig::StateClosedKey, std::string{ CoverStateMapper.map(CoverState::Closed) });
    config.set_override_if_not_null(CoverConfig::PayloadOpenKey, std::string{ CoverCommandMapper.map(CoverCommand::Open) });
    config.set_override_if_not_null(CoverConfig::PayloadCloseKey, std::string{ CoverCommandMapper.map(CoverCommand::Close) });
    config.set_override_if_not_null(CoverConfig::PayloadStopKey, std::string{ CoverCommandMapper.map(CoverCommand::Stop) });

    config.set_override(GenericEntityConfig::AvailibilityTopic, topics_.at(GenericEntityConfig::AvailibilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    client_.subscribe(subs_.begin(), subs_.end());
    client_.async_publish(fmt::format("homeassistant/cover/{}/config", unique_id_), config.parse());
  }

  void async_set_state(const CoverState& state)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

    client_.async_publish(topics_.at(CoverConfig::StateTopicKey), std::string{ CoverStateMapper.map(state) });
  }

  void async_set_availability(const Availability& availibility)
  {
    async_set_availibility(topics_.at(GenericEntityConfig::AvailibilityTopic), availibility);
  }

private:
  common::utils::StaticMap<std::string_view, std::string, 4> topics_{
    std::pair{ CoverConfig::StateTopicKey, topic(CoverConfig::TopicEntityName, CoverConfig::StateTopicValue) },
    std::pair{ CoverConfig::CommandTopicKey, topic(CoverConfig::TopicEntityName, CoverConfig::CommandTopicValue) },
    std::pair{ GenericEntityConfig::AvailibilityTopic, topic(CoverConfig::TopicEntityName, GenericEntityConfig::AvailibilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(CoverConfig::TopicEntityName, GenericEntityConfig::JsonAttributesTopic) },
  };
  std::unordered_map<std::string, PublishHandler_t> subs_{
    std::pair{ topic(CoverConfig::TopicEntityName, CoverConfig::CommandTopicValue), default_publish_handler() }
  };
};
}// namespace mgmt::home_assistant::mqttc