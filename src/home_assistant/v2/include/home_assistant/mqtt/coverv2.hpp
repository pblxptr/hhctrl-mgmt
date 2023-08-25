//
// Created by bielpa on 24.08.23.
//


#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

#include <utils/mapper.hpp>
#include <utils/static_map.hpp>

#include <home_assistant/mqtt/entityv2.hpp>

namespace mgmt::home_assistant::v2
{
enum class CoverState { Open,
  Opening,
  Closing,
  Closed
};

enum class CoverCommand { Open,
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
    static constexpr inline auto CommandTopic = std::string_view{"set"};
  };

  struct Property {
    static constexpr inline auto StateTopic = std::string_view{"state_topic"};
    static constexpr inline auto CommandTopic = std::string_view{"command_topic"};
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

constexpr static auto CoverCommandMapper = common::utils::Mapper{
  std::pair{ CoverCommand::Open, "open" },
  std::pair{ CoverCommand::Close, "close" },
  std::pair{ CoverCommand::Stop, "stop" }
};
using CoverCommandHandler_t = std::function<void(const CoverCommand&)>;

using CoverReceiveResult = std::variant<CoverCommand, CoverTiltCommand>;

template<class EntityClient>
class Cover : public Entity<EntityClient>
{
  using BaseType = Entity<EntityClient>;
  using BaseType::topic;
//  using BaseType::async_set_availability;
  using BaseType::async_publish;
//  using BaseType::async_subscribe;

public:
  using BaseType::unique_id;
  using BaseType::async_receive;

  Cover() = delete;
  Cover(std::string uid, EntityClient client)// TODO(pp): Consider passing EntityClient by rvalue ref
    : Entity<EntityClient>(CoverConfig::EntityName, std::move(uid), std::move(client))
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}, unique_id: {}", __FUNCTION__, unique_id());
  }

  // movable
  Cover(Cover&& rhs) noexcept = default;
  Cover& operator=(Cover&&) noexcept = default;
  // non-copyable
  Cover(const Cover&) = delete;
  Cover& operator=(const Cover&) = delete;

  ~Cover() = default;

  boost::asio::awaitable<std::error_code> async_set_config(EntityConfig config = EntityConfig{}, QOS qos = DefaultQoS)
  {
    common::logger::get(mgmt::home_assistant::Logger)->trace("Cover::{}", __FUNCTION__);

    config.set_override(CoverConfig::Property::StateTopic, topics_.at(CoverConfig::Property::StateTopic));
    config.set_override(CoverConfig::Property::CommandTopic, topics_.at(CoverConfig::Property::CommandTopic));
    config.set_override(CoverConfig::Property::StateOpening, std::string{ CoverStateMapper.map(CoverState::Opening) });
    config.set_override(CoverConfig::Property::StateOpen, std::string{ CoverStateMapper.map(CoverState::Open) });
    config.set_override(CoverConfig::Property::StateClosing, std::string{ CoverStateMapper.map(CoverState::Closing) });
    config.set_override(CoverConfig::Property::StateClosed, std::string{ CoverStateMapper.map(CoverState::Closed) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadOpen, std::string{ CoverCommandMapper.map(CoverCommand::Open) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadClose, std::string{ CoverCommandMapper.map(CoverCommand::Close) });
    config.set_override_if_not_null(CoverConfig::Property::PayloadStop, std::string{ CoverCommandMapper.map(CoverCommand::Stop) });

    config.set_override(GenericEntityConfig::AvailabilityTopic, topics_.at(GenericEntityConfig::AvailabilityTopic));
    config.set_override(GenericEntityConfig::JsonAttributesTopic, topics_.at(GenericEntityConfig::JsonAttributesTopic));
    config.set(GenericEntityConfig::JsonAttributesTemplate, "{{ value_json | tojson }}");

    co_return co_await BaseType::async_set_config(std::move(config));

//    co_await async_subscribe(subs_.begin(), subs_.end());
  }

  boost::asio::awaitable<Expected<CoverReceiveResult>> async_receive()
  {
    const auto packet = co_await BaseType::async_receive();

    if (!packet) {
      co_return Unexpected{ packet.error() };

//      const auto& error = packet.error();
//      std::visit(async_mqtt::overload {
//        [](const SubscriptionError& error) Expected<CoverReceiveResult> {
//          if (error.topic() == topics_) {
//            return Unexpected{CoverError::CommandSubscriptionFailure};
//          }
//          else if (error.topic() == topics) {
//            return Unexpected{CoverError::CommandTiltSubscriptionFailure};
//          }
//        },
//        [](const PublishError& error) -> Expected<CoverReceiveResult> {}
//      }, error);


    }





    const auto& value = packet.value();

    if (value.topic().find(topics_.at(CoverConfig::Property::CommandTopic))) {
      co_return Expected<CoverReceiveResult>{CoverCommandMapper.map(async_mqtt::to_string(value.payload()))};
    }
    else {
      co_return Unexpected{EntityError::UnknownPacket};
    }
  }

//  boost::asio::awaitable<void> async_set_state(const CoverState& state)
//  {
//    common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}, state: {}", __FUNCTION__, CoverStateMapper.map(state));
//    co_return;
//
////    co_await async_publish(topics_.at(CoverConfig::StateTopicKey), std::string{ CoverStateMapper.map(state) });
//  }

//  boost::asio::awaitable<void> async_set_availability(const Availability& availability)
//  {
//    common::logger::get(mgmt::home_assistant::Logger)->trace("Cover::{}", __FUNCTION__);
//    co_return;
//
////    co_await async_set_availability(topics_.at(GenericEntityConfig::AvailabilityTopic), availability);
//  }

private:
  std::string unique_id_;
  common::utils::StaticMap<std::string_view, std::string, 4> topics_{
    std::pair{ CoverConfig::Property::StateTopic, topic(CoverConfig::Default::StateTopic) },
    std::pair{ CoverConfig::Property::CommandTopic, topic(CoverConfig::Default::CommandTopic) },
    std::pair{ GenericEntityConfig::AvailabilityTopic, topic(GenericEntityConfig::AvailabilityTopic) },
    std::pair{ GenericEntityConfig::JsonAttributesTopic, topic(GenericEntityConfig::JsonAttributesTopic) },
  };
};
}
