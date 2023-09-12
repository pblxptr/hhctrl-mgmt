//
// Created by bielpa on 24.08.23.
//

#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>

#include <home_assistant/mqtt/entityv2.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::v2
{

enum class CoverState { Open,
  Opening,
  Closing,
  Closed
};

enum class CoverSwitchCommand { Open,
  Close,
  Stop
};

struct CoverTiltCommand
{
  CoverTiltCommand()
  {
    throw std::runtime_error{"Not implemented"};
  }
};

struct CoverConfig
{
  static constexpr inline auto EntityName = "cover";

  struct Default {
    static constexpr inline auto StateTopic = std::string_view{"state"};
    static constexpr inline auto SwitchCommandTopic = std::string_view{"set"};
  };

  struct Property {
    static constexpr inline auto StateTopic = std::string_view{"state_topic"};
    static constexpr inline auto SwitchCommandTopic = std::string_view{"command_topic"};
    static constexpr inline auto StateOpening = std::string_view{"state_opening"};
    static constexpr inline auto StateOpen = std::string_view{"state_open"};
    static constexpr inline auto StateClosing = std::string_view{"state_closing"};
    static constexpr inline auto StateClosed = std::string_view{"state_closed"};
    static constexpr inline auto PayloadOpen = std::string_view{"payload_open"};
    static constexpr inline auto PayloadClose = std::string_view{"payload_close"};
    static constexpr inline auto PayloadStop = std::string_view{"payload_stop"};
  };
};

constexpr static auto CoverStateMapper = common::utils::Mapper{
  std::pair{ CoverState::Open, "open" },
  std::pair{ CoverState::Opening, "opening" },
  std::pair{ CoverState::Closing, "closing" },
  std::pair{ CoverState::Closed, "closed" }
};

constexpr static auto CoverSwitchCommandMapper = common::utils::Mapper{
  std::pair{ CoverSwitchCommand::Open, "open" },
  std::pair{ CoverSwitchCommand::Close, "close" },
  std::pair{ CoverSwitchCommand::Stop, "stop" }
};

using CoverCommand = std::variant<CoverSwitchCommand, CoverTiltCommand>;

namespace detail {
  std::optional<CoverCommand> map_command(const auto& topics, const PublishPacket_t& pub_packet)
  {
    const auto topic = static_cast<std::string_view>(pub_packet.topic());

    auto payload = to_string(pub_packet.payload());

    if (topic == topics.at(CoverConfig::Property::SwitchCommandTopic)) {
      return CoverSwitchCommandMapper.map(payload);
    }

    //TODO(bielpa) Implement tilt here
    assert(0 && "Not implemented");
    }
  } // namespace detail


template<class EntityClient>
class Cover : public Entity<EntityClient>
{
  using BaseType = Entity<EntityClient>;
  using BaseType::topic;
  using BaseType::async_publish;

public:
  using BaseType::unique_id;
  using BaseType::async_set_availability;

  Cover() = delete;
  Cover(std::string uid, std::unique_ptr<EntityClient> client)
    : Entity<EntityClient>(CoverConfig::EntityName, std::move(uid), std::move(client))
  {
    logger::debug(logger::Entity, "Cover::{}, unique_id: {}", __FUNCTION__, unique_id());
  }

  // movable
  Cover(Cover&& rhs) noexcept = default;
  Cover& operator=(Cover&&) noexcept = default;
  // non-copyable
  Cover(const Cover&) = delete;
  Cover& operator=(const Cover&) = delete;

  ~Cover() = default;

  boost::asio::awaitable<Error> async_configure(EntityConfig config = EntityConfig{}, Pubopts_t pubopts = DefaultPubOpts)
  {
    logger::trace(logger::Entity, "Cover::{}", __FUNCTION__);

    config.set_override(CoverConfig::Property::SwitchCommandTopic, topics_.at(CoverConfig::Property::SwitchCommandTopic));
    config.set_override(CoverConfig::Property::StateTopic, topics_.at(CoverConfig::Property::StateTopic));
    config.set_override(CoverConfig::Property::StateOpening, std::string{ CoverStateMapper.map(CoverState::Opening) });
    config.set_override(CoverConfig::Property::StateOpen, std::string{ CoverStateMapper.map(CoverState::Open) });
    config.set_override(CoverConfig::Property::StateClosing, std::string{ CoverStateMapper.map(CoverState::Closing) });
    config.set_override(CoverConfig::Property::StateClosed, std::string{ CoverStateMapper.map(CoverState::Closed) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadOpen, std::string{ CoverSwitchCommandMapper.map(CoverSwitchCommand::Open) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadClose, std::string{ CoverSwitchCommandMapper.map(CoverSwitchCommand::Close) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadStop, std::string{ CoverSwitchCommandMapper.map(CoverSwitchCommand::Stop) });

    config.set_override(GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    // Set config
    if (const auto error = co_await BaseType::async_set_config(std::move(config), pubopts); error) {
      co_return error;
    }

    // Set subscriptions
    auto sub_topics = std::vector<std::string>{
      topics_.at(CoverConfig::Property::SwitchCommandTopic)
    };

    if (const auto error = co_await BaseType::async_subscribe(std::move(sub_topics)); error) {
      co_return error;
    }

    co_return Error{};
  }

  boost::asio::awaitable<Error> async_set_state(const CoverState& state, Pubopts_t pubopts = DefaultPubOpts)
  {
      logger::trace(logger::Entity, "Cover::{}", __FUNCTION__);

      co_return co_await BaseType::async_publish(topics_.at(CoverConfig::Property::StateTopic),
                                       std::string{ CoverStateMapper.map(state) }, pubopts
      );
  }

  boost::asio::awaitable<Expected<CoverCommand>> async_receive()
  {
      const auto& packet = co_await BaseType::async_receive();

      if (!packet) {
        co_return Unexpected{ packet.error() };
      }

      if (auto command = detail::map_command(topics_, packet.value()); command) {
        co_return *command;
      }
      co_return Unexpected { ErrorCode::UnknownPacket };
  }

private:
  std::string unique_id_;
  common::utils::StaticMap<std::string_view, std::string, 3> topics_{
    std::pair{ CoverConfig::Property::SwitchCommandTopic, topic(CoverConfig::Default::SwitchCommandTopic) },
    std::pair{ CoverConfig::Property::StateTopic, topic(CoverConfig::Default::StateTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(GenericEntityConfig::JsonAttributesTopic) },
  };

};
}
