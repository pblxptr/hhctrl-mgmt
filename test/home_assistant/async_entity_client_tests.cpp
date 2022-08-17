//
// Created by bielpa on 15.08.22.
//
#include <catch2/catch_all.hpp>
#include <random>

#include <home_assistant/mqtt/async_entity_client.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <coro/co_spawn.hpp>

namespace {
  auto rethrow = [](auto eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  };

  struct Client
  {
    virtual ~Client() = default;
    virtual boost::asio::awaitable<void> async_connect() = 0;
    virtual boost::asio::awaitable<void> async_subscribe() = 0;
    virtual boost::asio::awaitable<void> async_start() = 0;
  };

  constexpr auto NumberOfClients { 100 };
  constexpr auto NumberOfMessagesPerClient { 100 };
  size_t ReadyClients{0};
  size_t MessagesCounter{0};
  std::map<std::string, size_t> Sent{};
  std::map<std::string, size_t> Received{};
  boost::asio::io_context Ioc;
  std::vector<std::unique_ptr<Client>> Clients;


  boost::asio::awaitable<void> prepare_clients()
  {
    for (auto& client : Clients) {
      co_await client->async_connect();
    }
  }

  boost::asio::awaitable<void> run_clients()
  {
    auto executor = co_await boost::asio::this_coro::executor;

    for (auto& client : Clients) {
      boost::asio::co_spawn(executor, client->async_start(), common::coro::rethrow);
    }
    co_return;
  }

  auto get_id(std::size_t client_number)
  {
    return std::string{"client-"} + std::to_string(client_number);
  }

  auto rand_client_id()
  {
    static auto gen = std::default_random_engine{};
    auto dist = std::uniform_int_distribution<int>(0, NumberOfClients -1 );

    return get_id(dist(gen));
  }

  template<class Collection>
  void push(const std::string& client_id, Collection& collection)
  {
    ++collection[client_id];

    if (++MessagesCounter == (NumberOfClients * NumberOfMessagesPerClient) * 2) {
      Ioc.stop();
    }
  }

  void mark_ready()
  {
    if (++ReadyClients == NumberOfClients) {
      boost::asio::co_spawn(Ioc, run_clients(), rethrow);
    }
  }

  template<class Impl>
  class TestClient : public Client
  {
  public:
    TestClient(std::string id, Impl impl)
      : id_{ std::move(id) }
      , impl_{ std::move(impl) }
    {
      spdlog::debug("created client with id {}", id_);
    }

    boost::asio::awaitable<void> async_connect() override
    {
      impl_.set_error_handler([](auto) -> boost::asio::awaitable<void> {
        Ioc.stop();
        co_return;
      });
      impl_.set_connack_handler([this](auto&&...) {
        boost::asio::co_spawn(Ioc, async_subscribe(), rethrow);
      });
      const auto ret = co_await impl_.async_connect();

      spdlog::debug("async_entity_client_test, client {} connected", id_);

      assert(!ret);
    }

    boost::asio::awaitable<void> async_subscribe() override
    {
      const auto ret = co_await impl_.async_subscribe(handlers_.begin(), handlers_.end());

      assert(!ret);

      ready_ = true;
      mark_ready();

      spdlog::debug("async_entity_client_test, client {} subscribed", id_);
    }

    boost::asio::awaitable<void> async_start() override
    {
      assert(ready_);

      for (std::size_t i = 0; i < NumberOfMessagesPerClient; i++) {
        const auto id = rand_client_id();

        const auto ret = co_await impl_.async_publish(id, "tes message");
        assert(!ret);

        push(id, Sent);
      }
    }

  private:
    void handle_received()
    {
      push(id_, Received);
    }

  private:
    std::string id_;
    Impl impl_;
    bool ready_{ false };
    std::array<std::pair<std::string, std::function<void(MQTT_NS::buffer)>>, 1> handlers_ {
      std::pair { id_, [this](MQTT_NS::buffer /* val */) { handle_received(); }
      }};
  };
  template<class T>
  TestClient(std::string id, T impl) -> TestClient<T>;
  }

  using HomeAssistantClient_t = decltype(std::declval<mgmt::home_assistant::mqttc::EntityClientFactory>()
    .create_async_client(std::declval<std::string>())
  );
  using TestClient_t = TestClient<HomeAssistantClient_t>;


SCENARIO("Async clients can send and receive messages")
{
  spdlog::set_level(spdlog::level::debug);

  auto config = mgmt::home_assistant::mqttc::EntityClientConfig {
    .server_address = "192.168.0.115",
    .server_port = 1883
  };
  auto factory = mgmt::home_assistant::mqttc::EntityClientFactory{Ioc, config};
  for (std::size_t i = 0; i < NumberOfClients; i++) {
    auto client_id = get_id(i);
    Clients.push_back(std::make_unique<TestClient_t>(client_id, factory.create_async_client(client_id)));
  }

  boost::asio::co_spawn(Ioc, []() -> boost::asio::awaitable<void> {
      co_await prepare_clients();
  },
  rethrow);

  Ioc.run();

  for (std::size_t i = 0; i < NumberOfClients; i++) {
    const auto id = get_id(i);
    REQUIRE(Sent[id] == Received[id]);
  }

}