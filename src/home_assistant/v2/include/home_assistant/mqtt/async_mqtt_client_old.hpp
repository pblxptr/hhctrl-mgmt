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
#include <home_assistant/mqtt/error.hpp>
#include <home_assistant/mqtt/will.hpp>
#include <home_assistant/mqtt/expected.hpp>

namespace mgmt::home_assistant::v2
{
  constexpr inline auto DefaultProtocolVersion = async_mqtt::protocol_version::v3_1_1;

  using ProtocolVersion = async_mqtt::protocol_version;
  using QOS = async_mqtt::qos;
  using SubAckReturnCode = async_mqtt::suback_return_code;
  using TopicSubAckResult = std::tuple<std::string, SubAckReturnCode>;
  using SubscribeResult = std::tuple<std::error_code, std::vector<TopicSubAckResult>>;
  using PacketId = std::uint32_t;
  using PublishPacket = async_mqtt::v3_1_1::publish_packet;
  using PublishAckPacket = async_mqtt::v3_1_1::puback_packet;
  using SubscriptionAckPacket = async_mqtt::v3_1_1::suback_packet;

  using ReceiveResult = std::variant<PublishPacket, PublishAckPacket>;

  namespace detail {
    void dump_packet(const PublishPacket& pub_packet)
    {
      std::cout
        << "MQTT PUBLISH recv"
        << " pid:" << pub_packet.packet_id()
        << " topic:" << pub_packet.topic()
        << " payload:" << async_mqtt::to_string(pub_packet.payload())
        << " qos:" << pub_packet.opts().get_qos()
        << " retain:" << pub_packet.opts().get_retain()
        << " dup:" << pub_packet.opts().get_dup()
        << std::endl;
    }

    void dump_packet(const PublishAckPacket& puback_packet)
    {
      std::cout
        << "MQTT PUBACK recv"
        << " pid:" << puback_packet.packet_id()
        << std::endl;
    }
  } // namespace detail

  struct ClientConfig
  {
    std::string unique_id {};
    std::string username {};
    std::string password {};
    std::string host {};
    std::string port {};
    bool clean_session { true };
    uint16_t keep_alive {};
  };


  template <typename Executor, ProtocolVersion protocolVersion = DefaultProtocolVersion>
  class AsyncMqttClient
  {
    using Will = mgmt::home_assistant::mqttc::Will;
  public:
    AsyncMqttClient(Executor executor, ClientConfig config)
      : executor_{executor}
      , config_{std::move(config)}
      , ep_{protocolVersion, executor}
    {
    }

    void set_will(Will will)
    {
    }

    boost::asio::awaitable<Expected<ReceiveResult>> async_receive()
    {
      if (auto packet = co_await ep_.recv(boost::asio::use_awaitable)) {
        co_return packet.visit(
          async_mqtt::overload {
            [&](async_mqtt::v3_1_1::publish_packet pub_packet) -> Expected<ReceiveResult> {
              detail::dump_packet(pub_packet);
              return Expected<ReceiveResult>{std::move(pub_packet)};
            },
            [&](async_mqtt::v3_1_1::puback_packet puback_packet) -> Expected<ReceiveResult> {
              detail::dump_packet(puback_packet);
              return Expected<ReceiveResult>{std::move(puback_packet)};
            },
            [](auto const&) -> Expected<ReceiveResult> {
              return Unexpected {ClientError::UnknownPacket};
            }
          }
        );
      }
      else {
        co_return Unexpected{detail::map_error_code(packet.template get<async_mqtt::system_error>().code())};
      }
    }

    boost::asio::awaitable<std::error_code> async_connect()
    {
      using boost::asio::ip::tcp;
      using boost::asio::redirect_error;
      using boost::asio::use_awaitable;

      {
        // Resolve name
        auto resolver = tcp::resolver{executor_};
        auto res_ec = boost::system::error_code{};
        auto eps = co_await resolver.async_resolve(config_.host, config_.port, redirect_error(use_awaitable, res_ec));

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

//    boost::asio::awaitable<SubscribeResult> async_subscribe(const std::string& topic, QOS qos)
//    {
//      auto subs = std::vector<async_mqtt::topic_subopts>{
//        { async_mqtt::allocate_buffer(topic), qos }
//      };
//
//      if (auto error_code = co_await async_send_sub(subs)) {
//        co_return SubscribeResult { detail::map_error_code(error_code.code()), {} };
//      }
//
//      auto [error_code, suback_codes] = co_await async_recv_suback();
//
//      if (error_code) {
//        co_return SubscribeResult { detail::map_error_code(error_code.code()), {} };
//      }
//
//      auto suback_results = std::vector<TopicSubAckResult>();
//      std::ranges::transform(suback_codes, std::back_inserter(suback_results), [&subs, index = 0](auto ack_code) mutable {
//        return TopicSubAckResult { subs[index++].topic(), ack_code };
//      });
//
//      co_return SubscribeResult { std::error_code {}, std::move(suback_results) };
//    }

    template <typename Topic, typename Payload>
    boost::asio::awaitable<Expected<PacketId>> async_publish(Topic&& topic, Payload&& payload, QOS qos = QOS::at_least_once)
    {
      auto packet_id = qos == QOS::at_most_once
        ? async_mqtt::v3_1_1::puback_packet::packet_id_t { 0 }
        : *ep_.acquire_unique_packet_id();

      if (auto system_error = co_await ep_.send(
            async_mqtt::v3_1_1::publish_packet{
              packet_id,
              async_mqtt::allocate_buffer(std::forward<Topic>(topic)),
              async_mqtt::allocate_buffer(std::forward<Payload>(payload)),
              qos
            },
            boost::asio::use_awaitable
            )
      ) {
        co_return Unexpected{detail::map_error_code(system_error.code())};
      }

      co_return Expected<PacketId>{packet_id};
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

      if (async_mqtt::packet_variant packet_variant = co_await ep_.recv(use_awaitable)) {
        packet_variant.visit(
          async_mqtt::overload{
            [&](const async_mqtt::v3_1_1::connack_packet& packet) {
            },
            [](const auto&) {}
          });
        co_return async_mqtt::system_error{};
      } else {
        co_return packet_variant.get<async_mqtt::system_error>();
      }
    }

//    boost::asio::awaitable<async_mqtt::system_error> async_send_sub(std::vector<async_mqtt::topic_subopts> subs)
//    {
//      using boost::asio::use_awaitable;
//
//      //TODO(bielpa): Return packet id
//
//      if (auto system_error = co_await ep_.send(
//            async_mqtt::v3_1_1::subscribe_packet{
//              *ep_.acquire_unique_packet_id(),
//              async_mqtt::force_move(subs) // sub_entry variable is required to avoid g++ bug
//            },
//            use_awaitable)
//      ) {
//
//        co_return system_error;
//      }
//
//      co_return async_mqtt::error_code{};
//    }

//    boost::asio::awaitable<std::tuple<async_mqtt::system_error, std::vector<async_mqtt::suback_return_code>>> async_recv_suback()
//    {
//      using boost::asio::use_awaitable;
//
//      auto suback_codes = std::vector<async_mqtt::suback_return_code>{};
//
//      if (async_mqtt::packet_variant packet_variant = co_await ep_.recv(use_awaitable)) {
//        packet_variant.visit(
//          async_mqtt::overload {
//            [&](const async_mqtt::v3_1_1::suback_packet& packet) {
//              suback_codes = packet.entries();
//            },
//            [](auto const&) {}
//          }
//        );
//
//        co_return std::tuple { async_mqtt::system_error{}, std::move(suback_codes) };
//      }
//
//      else {
//        co_return std::tuple { packet_variant.get<async_mqtt::system_error>(), std::move(suback_codes) };
//      }
//    }

  private:
    Executor executor_; // TODO(bielpa): Possibly can be removed
    ClientConfig config_;
    async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt> ep_;
    Will will_ {};
  };
} // namespace mgmt::home_assistant::v2
