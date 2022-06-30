#pragma once

#include <mqtt/client.hpp>
#include <mqtt/buffer.hpp>
#include <spdlog/spdlog.h>

namespace mgmt::home_assistant::mqttc
{
  using PublishHandler_t = std::function<void(MQTT_NS::buffer)>;

  auto default_publish_handler() -> PublishHandler_t
  {
    return { [](auto&&) {
      spdlog::debug("default_publish_handler");
    }};
  }

  template<class Impl>
  class MqttEntityClient
  {
  public:
    MqttEntityClient(std::string uid, Impl impl)
      : impl_{std::move(impl)}
    {
      impl_->set_client_id(uid);
      impl_->set_clean_session(true);
      impl_->set_keep_alive_sec(30);
    }

    void async_connect()
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->connect();
    }

    template<class Handler>
    void set_connack_handler(Handler&& handler)
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_connack_handler(std::forward<Handler>(handler));
    }

    template<class Handler>
    void set_error_handler(Handler&& handler)
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_error_handler(std::forward<Handler>(handler));
    }

    template<class Handler>
    void set_close_handler(Handler&& handler)
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_close_handler(std::forward<Handler>(handler));
    }

    template<class Payload>
    void async_publish(const std::string& topic, Payload&& payload)
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->publish(topic, std::forward<Payload>(payload));
    }

    template<class Iterator>
    void subscribe(Iterator begin, Iterator end)
    {
      spdlog::debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_suback_handler([this, begin, end](auto packet_id, auto results){
        spdlog::debug("suback_handler");
        for (const auto& result : results) {
          spdlog::debug("  - suback_return_code: {}", MQTT_NS::suback_return_code_to_str(result));
        }

        impl_->set_publish_handler([this, begin, end](
          mqtt::optional<std::uint16_t> packet_id,
          mqtt::publish_options pubopts,
          mqtt::buffer topic_name,
          mqtt::buffer contents
        )
        {
          spdlog::debug("MqttEntityClient::{}::set_publish_handler", __FUNCTION__);

          auto topic_handler = std::find_if(begin, end, [&topic_name](auto&& v) {
            auto& [topic, handler] = v;
            return topic == topic_name;
          });

          if (topic_handler == end) {
            return false;
          }

          auto& [topic, handler] = *topic_handler;
          handler(std::move(contents));

          return true;
        });

        return true;
      });

      using MqttSub_t = std::tuple<MQTT_NS::string_view, MQTT_NS::subscribe_options>;

      const auto size = std::distance(begin, end);
      auto subs = std::vector<MqttSub_t>{};
      std::transform(begin, end, std::back_inserter(subs), [](auto&& sub) {
          const auto& [topic, _] = sub;
          return MqttSub_t { topic.c_str(), MQTT_NS::subscribe_options(MQTT_NS::qos::at_most_once) };
      });
      impl_->subscribe(subs);

    }

  private:
    Impl impl_;
  };

}