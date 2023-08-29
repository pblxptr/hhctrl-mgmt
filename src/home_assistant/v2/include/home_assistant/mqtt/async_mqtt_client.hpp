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
#include <tl/expected.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::v2
{
  using ProtocolVersion = async_mqtt::protocol_version;
  using QOS = async_mqtt::qos;
  using SubAckReturnCode = async_mqtt::suback_return_code;
  using PacketId = std::uint32_t;
  using Pubopts = async_mqtt::pub::opts;
  using ConnectPacket = async_mqtt::v3_1_1::connect_packet;
  using PublishPacket = async_mqtt::v3_1_1::publish_packet;
  using PublishAckPacket = async_mqtt::v3_1_1::puback_packet;
  using SubscriptionPacket = async_mqtt::v3_1_1::subscribe_packet;
  using SubscriptionAckPacket = async_mqtt::v3_1_1::suback_packet;
  using SubscriptionAckReturnCode = async_mqtt::suback_return_code;
  using ReceiveResult = std::variant<PublishPacket, PublishAckPacket, SubscriptionAckPacket>;
  using Will = async_mqtt::will;

  struct WillConfig
  {
    std::string topic {};
    std::string message {};
    Pubopts pubopts {};
  };

  constexpr inline auto DefaultQoS = QOS::at_least_once;
  constexpr inline auto DefaultProtocolVersion = ProtocolVersion::v3_1_1;

  inline bool any_suback_failure(const SubscriptionAckPacket& suback_packet)
  {
    return std::ranges::any_of(suback_packet.entries(), [](const auto& suback_code) {
      return suback_code == async_mqtt::suback_return_code::failure;
    });
  }

  inline bool supported_qos(QOS qos)
  {
    return qos == QOS::at_least_once || qos == QOS::at_most_once;
  }

  namespace detail {
    inline std::string to_string(const Will& will)
    {
      return fmt::format("topic: '{}', message: '{}', qos: '{}', retain: '{}'",
        static_cast<std::string_view>(will.topic()),
        static_cast<std::string_view>(will.message()),
        async_mqtt::qos_to_str(will.get_qos()),
        async_mqtt::pub::retain_to_str(will.get_retain())
      );
    }

    inline std::string to_string(const ConnectPacket& packet)
    {
      return fmt::format("client_id: '{}', keep_alive: '{}', clean_session: '{}', will: '{}'",
        static_cast<std::string_view>(packet.client_id()),
        packet.keep_alive(),
        packet.clean_session(),
        to_string(packet.get_will().value_or(Will{"null", "null"}))
      );
    }

    inline std::string to_string(const PublishPacket& packet)
    {
      return fmt::format("pid: '{}', topic: '{}', payload: '{}', qos: '{}', retain: '{}', dup: '{}'",
        packet.packet_id(),
        packet.topic().data(),
        async_mqtt::to_string(packet.payload()),
        async_mqtt::qos_to_str(packet.opts().get_qos()),
        async_mqtt::pub::retain_to_str(packet.opts().get_retain()),
        async_mqtt::pub::dup_to_str(packet.opts().get_dup()));
    }

    inline std::string to_string(const PublishAckPacket& packet)
    {
      return fmt::format("pid: '{}'", packet.packet_id());
    }

    inline std::string to_string(const SubscriptionPacket& packet)
    {
      auto format_subopt = [](const auto& subopt) mutable {
        return fmt::format("topic: '{}', qos: '{}'",
          static_cast<std::string_view>(subopt.topic()),
          async_mqtt::qos_to_str(subopt.opts().get_qos()));
      };

      auto&& new_range = packet.entries() | std::views::transform(format_subopt);

      return fmt::format("pid: {}, {}", packet.packet_id(), fmt::join(new_range, ","));
    }

    inline std::string to_string(const SubscriptionAckPacket& packet)
    {
      auto format_suback_retcode = [idx = 0](const auto& code) mutable {
        return fmt::format("- suback idx: '{}' suback retcode: '{}'", idx++, async_mqtt::suback_return_code_to_str(code));
      };

      auto&& new_range = packet.entries() | std::views::transform(format_suback_retcode);

      return fmt::format("pid: {}, \n{}", packet.packet_id(), fmt::join(new_range, "\n"));
    }
  }

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
  public:
    AsyncMqttClient(Executor executor, ClientConfig config)
      : executor_{executor}
      , config_{std::move(config)}
      , ep_{protocolVersion, executor}
    {
    }

    void set_will(const WillConfig& will)
    {
      will_ = Will {
        async_mqtt::allocate_buffer(will.topic),
        async_mqtt::allocate_buffer(will.message),
        will.pubopts
      };
    }

    boost::asio::awaitable<Expected<ReceiveResult>> async_receive()
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

        if (auto packet = co_await ep_.recv(boost::asio::use_awaitable)) {
          co_return packet.visit(
            async_mqtt::overload {
              [&](PublishPacket pub_packet) -> Expected<ReceiveResult> {
                logger::debug(logger::AsyncMqttClient, "Received Publish packet: '{}'", detail::to_string(pub_packet));
                return Expected<ReceiveResult>{std::move(pub_packet)};
              },
              [&](PublishAckPacket puback_packet) -> Expected<ReceiveResult> {
                logger::debug(logger::AsyncMqttClient, "Received PublishAck packet: '{}'", detail::to_string(puback_packet));
                return Expected<ReceiveResult>{std::move(puback_packet)};
              },
              [&](SubscriptionAckPacket subback_packet) -> Expected<ReceiveResult> {
                logger::debug(logger::AsyncMqttClient, "Received SubscribeAck packet: '{}'", detail::to_string(subback_packet));
                return Expected<ReceiveResult>{std::move(subback_packet)};
              },
              [](auto const&) -> Expected<ReceiveResult> {
                logger::warn(logger::AsyncMqttClient, "Unknown packet has been received");
                return Unexpected {ErrorCode::UnknownPacket, "Unknown packet has been received"};
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
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

      using boost::asio::ip::tcp;
      using boost::asio::redirect_error;
      using boost::asio::use_awaitable;

      {
        // Resolve name
        auto resolver = tcp::resolver{executor_};
        auto res_ec = boost::system::error_code{};
        auto eps = co_await resolver.async_resolve(config_.host, config_.port, redirect_error(use_awaitable, res_ec));

        if (res_ec) {
          logger::err(logger::AsyncMqttClient, "Error while resolving name: '{}'", res_ec.message());
          co_return detail::map_error_code(res_ec);
        }

        // Connect to broker
        auto con_ec = boost::system::error_code{};
        co_await boost::asio::async_connect(
          ep_.next_layer(),
          eps,
          redirect_error(use_awaitable, con_ec)
        );
        if (con_ec) {
          logger::err(logger::AsyncMqttClient, "Error while establishing connection error: '{}'", con_ec.message());
          co_return detail::map_error_code(con_ec);
        }
      }

      // Send ConnectPacket message
      if (auto error_code = co_await async_send_con(); error_code) {
        logger::err(logger::AsyncMqttClient, "Error while sending ConnectPacket: '{}'", error_code.message());
        co_return detail::map_error_code(error_code.code());
      }

      // Receive con ack
      if (auto error_code = co_await async_recv_conack(); error_code) {
        logger::err(logger::AsyncMqttClient, "Error while receiving ConnectPacketAck error: '{}'", error_code.message());
        co_return detail::map_error_code(error_code.code());
      }

      co_return std::error_code{};
    }

    boost::asio::awaitable<Expected<PacketId>> async_subscribe(std::vector<std::string> topics)
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

        // Prepare subscription
        auto subs = std::vector<async_mqtt::topic_subopts>{};
        subs.reserve(topics.size());

        std::ranges::transform(topics, std::back_inserter(subs), [](const auto& topic) {
          return async_mqtt::topic_subopts{ async_mqtt::allocate_buffer(topic), DefaultQoS };
        });

        // Send subscription request
        using boost::asio::use_awaitable;

        auto packet_id = acquire_packet_id();
        auto packet = SubscriptionPacket {
          packet_id,
          async_mqtt::force_move(subs)
        };
        logger::debug(logger::AsyncMqttClient, "Subscription packet: '{}'", detail::to_string(packet));

        if (auto system_error = co_await ep_.send(std::move(packet), use_awaitable)) {
          logger::err(logger::AsyncMqttClient, "Error while sending Subscription packet: '{}'",system_error.what());
          co_return Unexpected{detail::map_error_code(system_error.code())};
        }

      co_return Expected<PacketId>{packet_id};
    }

    template <typename Topic, typename Payload>
    boost::asio::awaitable<Expected<PacketId>> async_publish(Topic&& topic, Payload&& payload, QOS qos)
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

      assert(supported_qos(qos));

      if (!supported_qos(qos)) {
        co_return Unexpected{ErrorCode::QosNotSupported};
      }

      auto packet_id = acquire_packet_id(qos);
      auto packet = PublishPacket {
        packet_id,
        async_mqtt::allocate_buffer(std::forward<Topic>(topic)),
        async_mqtt::allocate_buffer(std::forward<Payload>(payload)),
        qos
      };
      logger::debug(logger::AsyncMqttClient, "Publish packet: '{}'", detail::to_string(packet));

      if (auto system_error = co_await ep_.send(std::move(packet), boost::asio::use_awaitable)) {
        logger::err(logger::AsyncMqttClient, "Error while sending Publish packet: '{}'",system_error.what());
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

      auto packet = ConnectPacket {
        config_.clean_session,
        config_.keep_alive,
        async_mqtt::allocate_buffer(config_.unique_id),
        will_,
        get_credential(config_.username),
        get_credential(config_.password)
      };
      logger::debug(logger::AsyncMqttClient, "Connect packet: '{}'", detail::to_string(packet));

      if (auto system_error = co_await ep_.send(std::move(packet), use_awaitable)) {
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

    auto acquire_packet_id()
    {
      return *ep_.acquire_unique_packet_id();
    }

    auto acquire_packet_id(QOS qos)
    {
      return qos == QOS::at_most_once
           ? async_mqtt::v3_1_1::puback_packet::packet_id_t { 0 }
           : *ep_.acquire_unique_packet_id();
    }

  private:
    Executor executor_; // TODO(bielpa): Possibly can be removed
    ClientConfig config_;
    async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt> ep_;
    std::optional<Will> will_ {std::nullopt};
  };
} // namespace mgmt::home_assistant::v2
