#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <home_assistant/mqtt/availibility.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/mqtt/availibility.hpp>

namespace mgmt::home_assistant::mqttc
{
  enum class CoverState { Open, Opening, Close, Closed };

  enum class CoverCommand { Open, Close, Stop };
  using CoverCommandHandler_t = std::function<void (const CoverCommand&)>;

  template<class EntityClient>
  class Cover
  {
  public:
    static constexpr inline auto StateTopic = "state";
    static constexpr inline auto CommandTopic = "command";
    static constexpr inline auto AvailibilityTopic = "availibility";
    static constexpr inline auto JsonAttributesTopic = "json_attributes";

    Cover(std::string unique_id, EntityClient client)
      : unique_id_{std::move(unique_id)}
      , client_{std::move(client)}
    {}

    std::string unique_id() const
    {
      return unique_id_;
    }

    void async_connect()
    {
      spdlog::debug("Cover::{}", __FUNCTION__);

      client_.async_connect();
    }

    void on_command(CoverCommandHandler_t handler)
    {
      spdlog::debug("Cover::{}", __FUNCTION__);

      subs_[fmt::format("cover_{}/{}", unique_id_, CommandTopic)] = [handler = std::move(handler)](auto&& content) {
        const auto& content_str = content;

        if (content_str == "open") {
          handler(CoverCommand::Open);
        }
        else if (content_str == "close") {
          handler(CoverCommand::Close);
        }
        else if (content_str == "stop") {
          handler(CoverCommand::Stop);
        }
      };
    }

    void async_set_config(EntityConfig config)
    {
      spdlog::debug("Cover::{}", __FUNCTION__);

      config.set("state_topic", fmt::format("cover_{}/{}", unique_id_, StateTopic));
      config.set("command_topic", fmt::format("cover_{}/{}", unique_id_, CommandTopic));
      config.set("availibility_topic", fmt::format("cover_{}/{}", unique_id_, AvailibilityTopic));
      config.set("json_attributes_topic", fmt::format("cover-{}/{}", unique_id_, JsonAttributesTopic));
      config.set("state_opening", "opening");
      config.set("state_open", "open");
      config.set("state_closing", "closing");
      config.set("state_closed", "closed");
      config.set("payload_open", "open");
      config.set("payload_close", "close");
      config.set("payload_stop", nullptr);
      config.set("json_attributes_template", "{{ value_json | tojson }}");

      client_.subscribe(subs_.begin(), subs_.end());

      fmt::print("{}", config.parse());

      client_.async_publish(fmt::format("homeassistant/cover/{}/config", unique_id_), config.parse());
    }

    void async_set_state(const CoverState& state)
    {
      spdlog::debug("Cover::{}", __FUNCTION__);

      auto state_str = std::string{};

      switch (state) {
        case CoverState::Close:
          state_str = "close";
          break;
        case CoverState::Closed:
          state_str = "closed";
          break;
        case CoverState::Open:
          state_str = "open";
          break;
        case CoverState::Opening:
          state_str = "opening";
          break;
      }

      client_.async_publish(fmt::format("cover_{}/{}", unique_id_, StateTopic), state_str);
    }

    void async_set_availibility(const Availibility& availibility)
    {
      spdlog::debug("Cover::{}", __FUNCTION__);

      auto availibility_str = std::string{};

      switch (availibility) {
        case Availibility::Offline:
          availibility_str = "offline";
          break;
        case Availibility::Online:
          availibility_str = "online";
          break;
      }

      client_.async_publish(fmt::format("cover_{}/{}", unique_id_, AvailibilityTopic), availibility_str);
    }
  private:
    std::string unique_id_;
    EntityClient client_;
    std::unordered_map<std::string, PublishHandler_t> subs_ {
      std::pair { fmt::format("cover_{}/{}", unique_id_, CommandTopic), default_publish_handler() }
    };
  };
}