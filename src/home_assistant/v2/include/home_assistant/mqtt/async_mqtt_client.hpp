//
// Created by bielpa on 20.08.23.
//

#pragma once

#include <ranges>
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
  using ProtocolVersion_t = async_mqtt::protocol_version;
  using Qos_t = async_mqtt::qos;
  using PacketId_t = std::uint32_t;
  using Pubopts_t = async_mqtt::pub::opts;
  using ConnectPacket_t = async_mqtt::v3_1_1::connect_packet;
  using PublishPacket_t = async_mqtt::v3_1_1::publish_packet;
  using PublishAckPacket_t = async_mqtt::v3_1_1::puback_packet;
  using SubscriptionPacket_t = async_mqtt::v3_1_1::subscribe_packet;
  using SubscriptionAckPacket_t = async_mqtt::v3_1_1::suback_packet;
  using ReceiveResult_t = std::variant<PublishPacket_t, PublishAckPacket_t, SubscriptionAckPacket_t>;
  using Will_t = async_mqtt::will;

  struct WillConfig
  {
    std::string topic {};
    std::string message {};
    Pubopts_t pubopts {};
  };

  constexpr inline auto DefaultQoS = Qos_t::at_least_once;
  constexpr inline auto DefaultPubOpts = Qos_t::at_least_once;
  constexpr inline auto DefaultProtocolVersion = ProtocolVersion_t::v3_1_1;

  inline bool any_suback_failure(const SubscriptionAckPacket_t& suback_packet)
  {
    return std::ranges::any_of(suback_packet.entries(), [](const auto& suback_code) {
      return suback_code == async_mqtt::suback_return_code::failure;
    });
  }

  inline bool supported_qos(Qos_t qos)
  {
    return qos == Qos_t::at_least_once || qos == Qos_t::at_most_once;
  }

  namespace detail {
    inline std::string to_string(const Will_t& will)
    {
      return fmt::format("topic: '{}', message: '{}', qos: '{}', retain: '{}'",
        static_cast<std::string_view>(will.topic()),
        static_cast<std::string_view>(will.message()),
        async_mqtt::qos_to_str(will.get_qos()),
        async_mqtt::pub::retain_to_str(will.get_retain())
      );
    }

    inline std::string to_string(const ConnectPacket_t& packet)
    {
      return fmt::format("client_id: '{}', keep_alive: '{}', clean_session: '{}', will: '{}'",
        static_cast<std::string_view>(packet.client_id()),
        packet.keep_alive(),
        packet.clean_session(),
        to_string(packet.get_will().value_or(Will_t{"null", "null"}))
      );
    }

    inline std::string to_string(const PublishPacket_t& packet)
    {
      return fmt::format("pid: '{}', topic: '{}', payload: '{}', qos: '{}', retain: '{}', dup: '{}'",
        packet.packet_id(),
        static_cast<std::string_view>(packet.topic()),
        async_mqtt::to_string(packet.payload()),
        async_mqtt::qos_to_str(packet.opts().get_qos()),
        async_mqtt::pub::retain_to_str(packet.opts().get_retain()),
        async_mqtt::pub::dup_to_str(packet.opts().get_dup()));
    }

    inline std::string to_string(const PublishAckPacket_t& packet)
    {
      return fmt::format("pid: '{}'", packet.packet_id());
    }

    inline std::string to_string(const SubscriptionPacket_t& packet)
    {
      auto format_subopt = [](const auto& subopt) mutable {
        return fmt::format("topic: '{}', qos: '{}'",
          static_cast<std::string_view>(subopt.topic()),
          async_mqtt::qos_to_str(subopt.opts().get_qos()));
      };

      auto&& new_range = packet.entries() | std::views::transform(format_subopt);

      return fmt::format("pid: {}, {}", packet.packet_id(), fmt::join(new_range, ","));
    }

    inline std::string to_string(const SubscriptionAckPacket_t& packet)
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

  template <typename Executor, ProtocolVersion_t protocolVersion = DefaultProtocolVersion>
  class AsyncMqttClient
  {
      using EndpointType = async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt>;
  public:
    AsyncMqttClient(Executor executor, ClientConfig config)
      : executor_{executor}
      , config_{std::move(config)}
      , ep_{protocolVersion, executor}
    {
    }

    void set_will(const WillConfig& will)
    {
      will_ = Will_t {
        async_mqtt::allocate_buffer(will.topic),
        async_mqtt::allocate_buffer(will.message),
        will.pubopts
      };
    }

    boost::asio::awaitable<Expected<ReceiveResult_t>> async_receive()
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

        if (auto packet = co_await ep_.recv(boost::asio::use_awaitable)) {
          co_return packet.visit(
            async_mqtt::overload {
              [&](PublishPacket_t pub_packet) -> Expected<ReceiveResult_t> {
                logger::debug(logger::AsyncMqttClient, "Received Publish packet: '{}'", detail::to_string(pub_packet));
                return Expected<ReceiveResult_t>{std::move(pub_packet)};
              },
              [&](PublishAckPacket_t puback_packet) -> Expected<ReceiveResult_t> {
                logger::debug(logger::AsyncMqttClient, "Received PublishAck packet: '{}'", detail::to_string(puback_packet));
                return Expected<ReceiveResult_t>{std::move(puback_packet)};
              },
              [&](SubscriptionAckPacket_t subback_packet) -> Expected<ReceiveResult_t> {
                logger::debug(logger::AsyncMqttClient, "Received SubscribeAck packet: '{}'", detail::to_string(subback_packet));
                return Expected<ReceiveResult_t>{std::move(subback_packet)};
              },
              [](auto const&) -> Expected<ReceiveResult_t> {
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

      // Send ConnectPacket_t message
      if (auto system_error = co_await async_send_con(); system_error) {
        logger::err(logger::AsyncMqttClient, "Error while sending ConnectPacket_t: '{}'", system_error.what());
        co_return detail::map_error_code(system_error.code());
      }

      // Receive con ack
      if (auto system_error = co_await async_recv_conack(); system_error) {
        logger::err(logger::AsyncMqttClient, "Error while receiving ConnectPacketAck error: '{}'", system_error.what());
        co_return detail::map_error_code(system_error.code());
      }

      co_return std::error_code{};
    }

    boost::asio::awaitable<Expected<PacketId_t>> async_subscribe(std::vector<std::string> topics)
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

        auto packet_id = co_await acquire_packet_id();
        SubscriptionPacket_t packet = SubscriptionPacket_t {
          packet_id,
          async_mqtt::force_move(subs)
        };
        logger::debug(logger::AsyncMqttClient, "Subscription packet: '{}'", detail::to_string(packet));

        if (auto system_error = co_await ep_.send(std::move(packet), use_awaitable)) {
          logger::err(logger::AsyncMqttClient, "Error while sending Subscription packet: '{}'",system_error.what());
          co_return Unexpected{detail::map_error_code(system_error.code())};
        }

      co_return Expected<PacketId_t>{packet_id};
    }

    template <typename Topic, typename Payload>
    boost::asio::awaitable<Expected<PacketId_t>> async_publish(Topic&& topic, Payload&& payload, Pubopts_t pubopts)
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

      assert(supported_qos(pubopts.get_qos()));

      if (!supported_qos(pubopts.get_qos())) {
        co_return Unexpected{ErrorCode::QosNotSupported};
      }

      auto packet_id = co_await acquire_packet_id(pubopts.get_qos());
      PublishPacket_t packet = PublishPacket_t {
        packet_id,
        async_mqtt::allocate_buffer(std::forward<Topic>(topic)),
        async_mqtt::allocate_buffer(std::forward<Payload>(payload)),
        pubopts
      };
      logger::debug(logger::AsyncMqttClient, "Sending Publish packet: '{}'", detail::to_string(packet));

      if (auto system_error = co_await ep_.send(std::move(packet), boost::asio::use_awaitable)) {
        logger::err(logger::AsyncMqttClient, "Error while sending Publish packet: '{}'",system_error.what());
        co_return Unexpected{detail::map_error_code(system_error.code())};
      }

      co_return Expected<PacketId_t>{packet_id};
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

      ConnectPacket_t packet = ConnectPacket_t {
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

    boost::asio::awaitable<EndpointType::packet_id_t> acquire_packet_id()
    {
        auto result = co_await ep_.acquire_unique_packet_id(boost::asio::use_awaitable);

        co_return *result;
    }

    boost::asio::awaitable<EndpointType::packet_id_t>  acquire_packet_id(Qos_t qos)
    {
        if (qos == Qos_t::at_most_once) {
            co_return EndpointType::packet_id_t { 0 };
        }

        co_return co_await acquire_packet_id();

        //      return qos == Qos_t::at_most_once
//           ? async_mqtt::v3_1_1::puback_packet::packet_id_t { 0 }
//           : *ep_.acquire_unique_packet_id();
    }

  private:
    Executor executor_; // TODO(bielpa): Possibly can be removed
    ClientConfig config_;
    EndpointType ep_;
    std::optional<Will_t> will_ {std::nullopt};
  };
} // namespace mgmt::home_assistant::v2
