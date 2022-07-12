#pragma once

#include <functional>
#include <spdlog/spdlog.h>
#include <string>
#include <home_assistant/availibility.hpp>
#include <home_assistant/mqtt/entity_config.hpp>

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

    Cover() = delete;
    Cover(std::string uid, EntityClient client)
      : unique_id_{std::move(uid)}
      , client_{std::move(client)}
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}, unique_id: {}", __FUNCTION__, unique_id_);
    }

    Cover(const Cover&) = delete;
    Cover& operator=(const Cover&) = delete;
    Cover(Cover&& rhs)
      : unique_id_{std::move(rhs.unique_id_)}
      , client_{std::move(rhs.client_)}
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::(Cover&&)");
    }

    Cover& operator=(Cover&&) = default;
    ~Cover()
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);
    }

    std::string unique_id() const
    {
      return unique_id_;
    }

    void connect()
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      client_.connect();
    }

    template<class Handler>
    void set_ack_handler(Handler handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      client_.set_connack_handler(std::move(handler));
    }

    template<class Handler>
    void set_error_handler(Handler handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      client_.set_error_handler(std::move(handler));
    }

    template<class Handler>
    void set_close_handler(Handler handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      client_.set_close_handler(handler);
    }

    void on_command(CoverCommandHandler_t handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      //TODO: Use mapper here

      subs_[fmt::format("cover_{}/{}", unique_id_, CommandTopic)] = [handler = std::move(handler)](auto&& content) {
        if (content == "open") {
          handler(CoverCommand::Open);
        }
        else if (content == "close") {
          handler(CoverCommand::Close);
        }
        else if (content == "stop") {
          handler(CoverCommand::Stop);
        }
      };
    }

    void async_set_config(EntityConfig config)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      config.set("state_topic", fmt::format("cover_{}/{}", unique_id_, StateTopic));
      config.set("command_topic", fmt::format("cover_{}/{}", unique_id_, CommandTopic));
      config.set("availibility_topic", fmt::format("cover_{}/{}", unique_id_, AvailibilityTopic));
      config.set("json_attributes_topic", fmt::format("cover_{}/{}", unique_id_, JsonAttributesTopic));
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
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

      //TODO: use mapper here

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
      common::logger::get(mgmt::home_assistant::Logger)->debug("Cover::{}", __FUNCTION__);

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