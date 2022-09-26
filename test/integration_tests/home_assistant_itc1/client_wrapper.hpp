#pragma once

#include <boost/asio/awaitable.hpp>
#include <spdlog/spdlog.h>
#include <home_assistant/mqtt/async_entity_client.hpp>
#include "test_context.hpp"

struct ClientWrapper
{
  virtual ~ClientWrapper() = default;
  virtual boost::asio::awaitable<void> async_connect() = 0;
  virtual boost::asio::awaitable<void> async_subscribe() = 0;
  virtual boost::asio::awaitable<void> async_start() = 0;
};

template<class Impl>
class AsyncClientWrapper : public ClientWrapper
{
public:
  AsyncClientWrapper(
    mgmt::home_assistant::mqttc::AsyncMqttEntityClient<Impl> client,
    TestContext& context)
    : sut_{ std::move(client) }
    , context_{ context }
  {
    spdlog::debug("Created AsyncClientWrapper  {}", sut_.client_id());
  }

  boost::asio::awaitable<void> async_connect() override
  {
    sut_.set_error_handler([this](auto) -> boost::asio::awaitable<void> {
      context_.get().fail(sut_.client_id());
      co_return;
    });
    sut_.set_connack_handler([this](auto&&...) -> boost::asio::awaitable<void> {
      co_await async_subscribe();
    });
    const auto ret = co_await sut_.async_connect();
    assert(!ret);
    spdlog::debug(ret.message());

    spdlog::debug("AsyncClientWrapper: {} connected", sut_.client_id());
  }

  boost::asio::awaitable<void> async_subscribe() override
  {
    const auto ret = co_await sut_.async_subscribe(handlers_.begin(), handlers_.end());
    assert(!ret);

    context_.get().mark_ready(sut_.client_id());

    spdlog::debug("AsyncClientWrapper: {} subscribed", sut_.client_id());
  }

  boost::asio::awaitable<void> async_start() override
  {
    assert(context_.get().marked_ready(sut_.client_id()));

    const auto number_of_messages_per_client = context_.get().number_of_messages_per_client();

    for (std::size_t i = 0; i < number_of_messages_per_client; i++) {
      const auto id = context_.get().rand_client_id();

      const auto ret = co_await sut_.async_publish(id, "test message");
      assert(!ret);

      context_.get().notify_sent_for(id);
    }
    co_return;
  }

private:
  void handle_received()
  {
    spdlog::debug("AsyncClientWrapper: {} received", sut_.client_id());

    context_.get().notify_received_by(sut_.client_id());
  }

private:
  mgmt::home_assistant::mqttc::AsyncMqttEntityClient<Impl> sut_;
  std::reference_wrapper<TestContext> context_;
  std::array<std::pair<std::string, std::function<void(MQTT_NS::buffer)>>, 1> handlers_{
    std::pair{ sut_.client_id(), [this](MQTT_NS::buffer /* val */) { handle_received(); } }
  };
};

template<class T>
AsyncClientWrapper(std::string id, T impl) -> AsyncClientWrapper<T>;
