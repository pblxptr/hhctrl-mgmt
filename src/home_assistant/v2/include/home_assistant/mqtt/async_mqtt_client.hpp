//
// Created by bielpa on 20.08.23.
//

#pragma once

#include <async_mqtt/all.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/connect.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <home_assistant/mqtt/error.h>

namespace mgmt::home_assistant::v2
{
  constexpr inline auto DefaultProtocolVersion = async_mqtt::protocol_version::v3_1_1;

  struct ClientConfig
  {
    std::string unique_id {};
    std::string username {};
    std::string password {};
    bool clean_session { true };
    uint16_t keep_alive {};
  };

  template <typename Executor, async_mqtt::protocol_version ProtocolVersion = DefaultProtocolVersion>
  class AsyncMqttClient
  {
  public:
    AsyncMqttClient(Executor executor, ClientConfig config)
      : executor_{executor}
      , config_{std::move(config)}
      , ep_{ProtocolVersion, executor}
    {
    }

    boost::asio::awaitable<std::error_code> async_connect(std::string_view host, std::string_view port)
    {
      using boost::asio::ip::tcp;
      using boost::asio::redirect_error;
      using boost::asio::use_awaitable;

      {
        // Resolve name
        auto resolver = tcp::resolver{executor_};
        auto res_ec = boost::system::error_code{};
        auto eps = co_await resolver.async_resolve(host, port, redirect_error(use_awaitable, res_ec));

        if (res_ec) {
          co_return detail::map_error_code(res_ec);
        }

        auto con_ec = boost::system::error_code{};

        // Connect to broker
        co_await boost::asio::async_connect(
          ep_.next_layer(),
          eps,
          redirect_error(use_awaitable, con_ec)
        );
        if (con_ec) {
          co_return detail::map_error_code(con_ec);
        }
      }

      // Send connection establish message
      if (auto error_code = co_await async_send_con(); error_code) {
        co_return detail::map_error_code(error_code.code());
      }

      // Receive con ack
      if (auto error_code = co_await async_recv_conack(); error_code) {
        co_return detail::map_error_code(error_code.code());
      }

      co_return std::error_code{};
    }

  private:
    boost::asio::awaitable<async_mqtt::system_error> async_send_con()
    {
      using boost::asio::use_awaitable;
      using boost::asio::redirect_error;

      auto get_credential = [](const auto& credential) -> std::optional<async_mqtt::buffer>
      {
        if (credential.empty()) {
          return std::nullopt;
        }
        return std::optional{async_mqtt::allocate_buffer(credential)};
      };


      if (auto system_error = co_await ep_.send(
            async_mqtt::v3_1_1::connect_packet{
              config_.clean_session,   // clean_session
              config_.keep_alive, // keep_alive
              async_mqtt::allocate_buffer(config_.unique_id),
              async_mqtt::nullopt, // will
              get_credential(config_.username),
              get_credential(config_.password)
            },
            use_awaitable
            )) {
        std::cout << "MQTT CONNECT send error:" << system_error.what() << std::endl;
        co_return system_error;
      }

      co_return async_mqtt::system_error{};
    }

    boost::asio::awaitable<async_mqtt::system_error> async_recv_conack()
    {
      using boost::asio::use_awaitable;
      using boost::asio::redirect_error;

      if (async_mqtt::packet_variant pv = co_await ep_.recv(use_awaitable)) {
        pv.visit(
          async_mqtt::overload{
            [&](const async_mqtt::v3_1_1::connack_packet& p) {
              std::cout
                << "MQTT CONNACK recv"
                << " sp:" << p.session_present()
                << std::endl;
            },
            [](const auto&) {}
          });
        co_return async_mqtt::system_error{};
      } else {
        std::cout
          << "MQTT CONNACK recv error:"
          << pv.get<async_mqtt::system_error>().what()
          << std::endl;
        co_return pv.get<async_mqtt::system_error>();
      }
    }
  private:
    Executor executor_;
    ClientConfig config_;
    async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt> ep_;
  };
} // namespace mgmt::home_assistant::v2
