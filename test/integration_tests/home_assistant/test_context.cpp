//
// Created by bielpa on 09.09.22.
//
#include <random>
#include <cassert>
#include <numeric>
#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/client_config.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>

#include "test_context.hpp"
#include "client_wrapper.hpp"

namespace {

auto build_client_id(std::size_t client_number)
{
  return std::string{ "client-" } + std::to_string(client_number);
}
}

TestContext::TestContext(boost::asio::io_context& ioc, TestParams params)
  : ioc_{ioc}
  , params_{std::move(params)}
{}

//Done on purpose, explanation in header
TestContext::~TestContext()
{}

size_t TestContext::number_of_messages_per_client() const
{
  return params_.number_of_messages_per_client;
}

std::string TestContext::rand_client_id() const
{
  static auto gen = std::default_random_engine{};
  auto dist = std::uniform_int_distribution<int>(0, params_.number_of_clients - 1);

  return build_client_id(dist(gen));
}

void TestContext::fail(const std::string& client_id)
{
  spdlog::debug("Test interrupted by client: {}", client_id);

  assert(0);
}

void TestContext::mark_ready(const std::string& client_id)
{
  messages_[client_id] = {};

  if (messages_.size() == params_.number_of_clients) {
    run_clients();
  }
}

bool TestContext::marked_ready(const std::string& client_id)
{
  return messages_.contains(client_id);
}

void TestContext::notify_received_by(const std::string& client_id)
{
  assert(marked_ready(client_id));

  ++messages_[client_id].received_by;

  if (can_finish()) {
    finish();
  }
}

void TestContext::notify_sent_for(const std::string& client_id)
{
  assert(marked_ready(client_id));

  ++messages_[client_id].sent_for;

  if (can_finish()) {
    finish();
  }
}

void TestContext::run()
{
  auto config = mgmt::home_assistant::mqttc::EntityClientConfig{
    .server_address = params_.server_address,
    .server_port = params_.server_port
  };
  auto factory = mgmt::home_assistant::mqttc::EntityClientFactory{ ioc_, config };
  for (std::size_t i = 0; i < params_.number_of_clients; i++) {
    auto client_id = build_client_id(i);

    clients_.push_back(std::unique_ptr<ClientWrapper>{new AsyncClientWrapper{factory.create_async_client(client_id), *this}});
  }

  boost::asio::co_spawn(
    ioc_, [this]() -> boost::asio::awaitable<void> {
      for (auto& client : clients_) {
        co_await client->async_connect();
      }
    },
    common::coro::rethrow);

  ioc_.run();
}

void TestContext::run_clients()
{
  for (auto& client : clients_) {
    boost::asio::co_spawn(ioc_, client->async_start(), common::coro::rethrow);
  }
}

bool TestContext::can_finish() const
{
  size_t counter = std::accumulate(messages_.begin(), messages_.end(), 0, [](size_t value, auto&& v) {
    return value + v.second.sent_for + v.second.received_by;
  });

  // Messages contain sent_for, and received_by properties. Because we multiply number of clients
  // by number of messages per client, we have to multiply it by 2 in order to take received_by into account.
  if (counter == (params_.number_of_clients * params_.number_of_messages_per_client) * 2) {
    return true;
  }

  return false;
}

void TestContext::finish() const
{
  ioc_.stop();

  REQUIRE(messages_.size() == params_.number_of_clients);
  REQUIRE(clients_.size() == params_.number_of_clients);

  for (auto&& [k, v] : messages_) {
    REQUIRE(v.sent_for == v.received_by);
  }
}
