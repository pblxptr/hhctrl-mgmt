#pragma once

#include <boost/asio/steady_timer.hpp>

#include <mqtt/async_client.hpp>
#include <mqtt/buffer.hpp>
#include <coro/awaitable_adapter.hpp>
#include <coro/co_spawn.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/mqtt/entity_error.hpp>
#include <home_assistant/mqtt/client_config.hpp>

namespace mgmt::home_assistant::mqttc {
using PublishHandler_t = std::function<void(MQTT_NS::buffer)>;
using ErrorHandler_t = std::function<void(const EntityError&)>;

inline auto default_publish_handler() -> PublishHandler_t
{
  return { [](auto&&) {
    common::logger::get(mgmt::home_assistant::Logger)->debug("default_publish_handler");
  } };
}

template<class Impl>
class AsyncMqttEntityClient
{
  struct Reconnect {
    int attempt {};
    int max_attempts { 5 };
    std::chrono::seconds reconnect_delay = { std::chrono::seconds(3) };
    boost::asio::steady_timer timer;
  };
public:
  AsyncMqttEntityClient() = delete;
  AsyncMqttEntityClient(std::string uid, Impl impl, const EntityClientConfig& config)
    : impl_{ std::move(impl) }
    , reconnect_{
      .max_attempts = config.max_reconnect_attempts,
      .reconnect_delay = config.reconnect_delay,
      .timer = boost::asio::steady_timer{ impl_->socket()->get_executor() }
    }
  {
    impl_->set_client_id(uid);
    impl_->set_clean_session(true);
    impl_->set_keep_alive_sec(config.keep_alive_interval);
  }

  boost::asio::awaitable<MQTT_NS::error_code> async_connect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    const auto ec = co_await common::coro::awaitable_call<MQTT_NS::error_code>([this](auto&& h) {
      impl_->async_connect(std::forward<decltype(h)>(h));
    });

    if (ec && not (co_await reconnect())) {
      co_await on_error(ec);
    }

    co_return ec;
  }

  template<class Handler>
  void set_connack_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    impl_->set_connack_handler([this, client_handler = std::move(handler)](bool sp, auto rc) mutable {
      return on_ack(sp, rc, std::move(client_handler));
    });
  }

  template<class Handler>
  void set_error_handler(Handler handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    error_handler_ = std::move(handler);

    impl_->set_error_handler([this](const auto& ec) {
      boost::asio::co_spawn(impl_->socket()->get_executor(), on_error(ec), common::coro::rethrow);
    });
    impl_->set_close_handler([this]() {
      boost::asio::co_spawn(impl_->socket()->get_executor(), on_close(), common::coro::rethrow);
    });
  }

  template<class Payload>
  boost::asio::awaitable<MQTT_NS::error_code> async_publish(const std::string& topic, Payload&& payload)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    co_return co_await common::coro::awaitable_call<MQTT_NS::error_code>([this, &topic, payload = std::move(payload)](auto&& h) mutable {
      impl_->async_publish(topic, std::move(payload), MQTT_NS::qos::at_most_once, std::forward<decltype(h)>(h));
    });
  }

  template<class Iterator>
  boost::asio::awaitable<MQTT_NS::error_code> async_subscribe(Iterator begin, Iterator end)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    impl_->set_suback_handler([this, begin, end](auto packet_id, auto results) {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::suback_handler, packet_id: {}", packet_id);
      for (const auto& result : results) {
        common::logger::get(mgmt::home_assistant::Logger)->debug("  - suback_return_code: {}", MQTT_NS::suback_return_code_to_str(result));
      }

      impl_->set_publish_handler([this, begin, end](
                                   mqtt::optional<std::uint16_t> packet_id,
                                   [[maybe_unused]] mqtt::publish_options pubopts,
                                   mqtt::buffer topic_name,
                                   mqtt::buffer contents) {
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

    using MqttSub_t = std::tuple<std::string, MQTT_NS::subscribe_options>;

    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, sending sub", __FUNCTION__);

    auto subs = std::vector<MqttSub_t>{};
    std::transform(begin, end, std::back_inserter(subs), [](auto&& sub) {
      const auto& [topic, _] = sub;
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, subscribe topic: {}", __FUNCTION__, topic);

      return MqttSub_t{ topic, MQTT_NS::subscribe_options(MQTT_NS::qos::at_most_once) };
    });
    co_return co_await common::coro::awaitable_call<MQTT_NS::error_code>([this, &subs](auto&& h) {
      impl_->async_subscribe(subs, std::forward<decltype(h)>(h));
    });
  }

private:
  boost::asio::awaitable<bool> reconnect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    if (++reconnect_.attempt > reconnect_.max_attempts) {
      co_return false;
    }

    auto ec = boost::system::error_code{};
    reconnect_.timer.expires_after(reconnect_.reconnect_delay);
    co_await reconnect_.timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
    if (!ec) {
      common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::reconnect, attempt: {}/{}",
        reconnect_.attempt, reconnect_.max_attempts
      );
      co_await async_connect();
    }

    co_return true;
  }

  template<class Handler>
  bool on_ack(bool sp, mqtt::connect_return_code connack_return_code, Handler&& client_handler)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, session present: {}, conack ret code: {}", __FUNCTION__, sp, MQTT_NS::connect_return_code_to_str(connack_return_code));

    reconnect_.attempt = 0;
    client_handler();

    return true;
  }

  boost::asio::awaitable<void> on_error(const boost::system::error_code& ec)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}, ec: {}", __FUNCTION__, ec.message());

    if (not (co_await reconnect())) {
      error_handler_(EntityError{ EntityError::Code::Undefined, ec.message() });
    }
  }

  boost::asio::awaitable<void> on_close()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("MqttEntityClient::{}", __FUNCTION__);

    if (not (co_await reconnect())) {
      error_handler_(EntityError{ EntityError::Code::Disconnected, "Connection has been closed" });
    }
  }

private:
  Impl impl_;
  ErrorHandler_t error_handler_;
  Reconnect reconnect_;
};

}// namespace mgmt::home_assistant::mqttc