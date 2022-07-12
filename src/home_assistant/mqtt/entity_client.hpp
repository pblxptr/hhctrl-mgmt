#pragma once

#include <mqtt/client.hpp>
#include <mqtt/buffer.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::mqttc
{
  using PublishHandler_t = std::function<void(MQTT_NS::buffer)>;

  auto default_publish_handler() -> PublishHandler_t
  {
    return { [](auto&&) {
      common::logger::get(mgmt::home_assistant::Logger)->debug("default_publish_handler");
    }};
  }

  template<class Impl>
  class MqttEntityClient
  {
  public:
    MqttEntityClient() = delete;
    MqttEntityClient(std::string uid, Impl impl)
      : impl_{std::move(impl)}
    {
      impl_->set_client_id(uid);
      impl_->set_clean_session(true);
      impl_->set_keep_alive_sec(30);
    }

    void connect()
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->connect();
    }

    template<class Handler>
    void set_connack_handler(Handler&& handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_connack_handler([this, client_handler = std::forward<Handler>(handler)](bool sp, auto rc) mutable {
        return on_ack(sp, rc, std::forward<Handler>(client_handler));
      });
    }

    template<class Handler>
    void set_error_handler(Handler&& handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_error_handler([this, client_handler = std::forward<Handler>(handler)](const auto& ec) mutable {
        on_error(ec, std::forward<Handler>(client_handler));
      });
    }

    template<class Handler>
    void set_close_handler(Handler&& handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_close_handler([this, client_handler = std::forward<Handler>(handler)]() mutable {
        on_close(std::forward<Handler>(client_handler));
      });
    }

    template<class Payload>
    void async_publish(const std::string& topic, Payload&& payload)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->publish(topic, std::forward<Payload>(payload));
    }

    template<class Iterator>
    void subscribe(Iterator begin, Iterator end)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      impl_->set_suback_handler([this, begin, end](auto packet_id, auto results){
        common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::suback_handler, packet_id: {}", packet_id);
        for (const auto& result : results) {
          common::logger::get(mgmt::home_assistant::Logger)->debug("  - suback_return_code: {}", MQTT_NS::suback_return_code_to_str(result));
        }

        impl_->set_publish_handler([this, begin, end](
          mqtt::optional<std::uint16_t> packet_id,
          [[maybe_unused]] mqtt::publish_options pubopts,
          mqtt::buffer topic_name,
          mqtt::buffer contents
        )
        {
          common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}::publish_handler, packet id: {}", __FUNCTION__, packet_id.value_or(0));

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

      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, sending sub", __FUNCTION__);

      auto subs = std::vector<MqttSub_t>{};
      std::transform(begin, end, std::back_inserter(subs), [](auto&& sub) {
          const auto& [topic, _] = sub;
          common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, subscribe topic: {}", __FUNCTION__, topic);

          return MqttSub_t { topic.c_str(), MQTT_NS::subscribe_options(MQTT_NS::qos::at_most_once) };
      });
      impl_->subscribe(subs);
    }
  private:
    template<class Handler>
    bool on_ack(bool sp, mqtt::connect_return_code connack_return_code, Handler&& client_handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, session present: {}, conack ret code: {}",
        __FUNCTION__, sp, MQTT_NS::connect_return_code_to_str(connack_return_code)
      );

      client_handler();

      return true;
    }

    template<class Handler>
    void on_error(const boost::system::error_code& ec, Handler&& client_handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, ec: {}", __FUNCTION__, ec.message());

      client_handler();
    }

    template<class Handler>
    void on_close(Handler&& client_handler)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

      client_handler();
    }

  private:
    Impl impl_;
  };

}