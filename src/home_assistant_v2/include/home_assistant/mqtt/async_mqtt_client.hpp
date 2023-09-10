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
#include <home_assistant/mqtt/opts.hpp>
#include <home_assistant/mqtt/expected.hpp>
#include <home_assistant/mqtt/logger.hpp>

namespace mgmt::home_assistant::v2
{
  using ProtocolVersion_t = async_mqtt::protocol_version;
  using PacketId_t = std::uint32_t;
  using ConnectPacket_t = async_mqtt::v3_1_1::connect_packet;
  using ConnectAckPacket_t = async_mqtt::v3_1_1::connack_packet;
  using PublishPacket_t = async_mqtt::v3_1_1::publish_packet;
  using PublishAckPacket_t = async_mqtt::v3_1_1::puback_packet;
  using SubscriptionPacket_t = async_mqtt::v3_1_1::subscribe_packet;
  using SubscriptionAckPacket_t = async_mqtt::v3_1_1::suback_packet;
  using PingResponsePacket = async_mqtt::v3_1_1::pingresp_packet;
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

    inline std::string to_string(const ConnectAckPacket_t& packet)
    {
        return fmt::format("session present: '{}', return code: '{}'", packet.session_present(), async_mqtt::connect_return_code_to_str(packet.code()));
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
    int max_attempts = { 5 };
    std::chrono::seconds reconnect_delay = { std::chrono::seconds(5) };
  };

  template <ProtocolVersion_t protocolVersion = DefaultProtocolVersion>
  class AsyncMqttClient
  {
      using EndpointType = async_mqtt::endpoint<async_mqtt::role::client, async_mqtt::protocol::mqtt>;
      struct Reconnect
      {
          int attempt{};
          int max_attempts{ 5 };
          std::chrono::seconds reconnect_delay = { std::chrono::seconds(10) };
          boost::asio::steady_timer timer;
      };

  public:
    template <typename Executor>
    AsyncMqttClient(Executor executor, const ClientConfig& config)
      : config_{config}
      , ep_{protocolVersion, executor}
      , reconnect_ {
        .max_attempts = config.max_attempts,
        .reconnect_delay = config.reconnect_delay,
        .timer = boost::asio::steady_timer{ executor }
      }
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

    bool connected() const
    {
        return connected_;
    }

    boost::asio::awaitable<Expected<ReceiveResult_t>> async_receive()
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

        if (!connected()) {
            co_return Unexpected { ErrorCode::Disconnected, "Cannot receive on disconnected client " };
        }

            while (true) {
                if (auto packet = co_await ep_.recv(boost::asio::use_awaitable)) {
                    auto type = packet.type();
                    if (!type) {
                        co_return Unexpected {ErrorCode::UnknownPacket, "Chuj"};
                    }

                    switch (type.value()) {
                        case async_mqtt::control_packet_type::publish:
                            co_return Expected<ReceiveResult_t>{packet.template get<PublishPacket_t>()};
                        case async_mqtt::control_packet_type::puback:
                            co_return Expected<ReceiveResult_t>{packet.template get<PublishAckPacket_t>()};
                        case async_mqtt::control_packet_type::suback:
                            co_return Expected<ReceiveResult_t>{packet.template get<SubscriptionAckPacket_t>()};
                        default:
                            logger::debug(logger::AsyncMqttClient, "Packet received: {}", async_mqtt::control_packet_type_to_str(type.value()));
                    }
                    logger::debug(logger::AsyncMqttClient, "Packet received: {}", async_mqtt::control_packet_type_to_str(type.value()));



//                    if (std::holds_alternative<PublishPacket_t>(packet.get())) {
//                        auto& pub_packet = std::get<PublishPacket_t>(std::move(packet));
//                            logger::debug(logger::AsyncMqttClient, "Received Publish packet: '{}'",
//                                          detail::to_string(pub_packet));
//                            co_return Expected<ReceiveResult_t>{std::move(pub_packet)};
//                    }
//                    else if (std::holds_alternative<PublishAckPacket_t>(packet.get())) {
//                        auto& puback_packet = std::get<PublishAckPacket_t>(std::move(packet));
//                        logger::debug(logger::AsyncMqttClient, "Received PublishAckPacket_t packet: '{}'",
//                                      detail::to_string(puback_packet));
//                        co_return Expected<ReceiveResult_t>{std::move(puback_packet)};
//                    }
//                    else if (std::holds_alternative<SubscriptionAckPacket_t>(packet.get())) {
//                        auto& subback_packet = std::get<SubscriptionAckPacket_t>(std::move(packet));
//                        logger::debug(logger::AsyncMqttClient, "Received SubscriptionAckPacket_t packet: '{}'",
//                                      detail::to_string(subback_packet));
//                        co_return Expected<ReceiveResult_t>{std::move(subback_packet)};
//                    }



//                    co_return packet.visit(
//                            async_mqtt::overload{
//                                    [&](PublishPacket_t pub_packet) -> Expected<ReceiveResult_t> {
//                                        logger::debug(logger::AsyncMqttClient, "Received Publish packet: '{}'",
//                                                      detail::to_string(pub_packet));
//                                        return Expected<ReceiveResult_t>{std::move(pub_packet)};
//                                    },
//                                    [&](PublishAckPacket_t puback_packet) -> Expected<ReceiveResult_t> {
//                                        logger::debug(logger::AsyncMqttClient, "Received PublishAck packet: '{}'",
//                                                      detail::to_string(puback_packet));
//                                        return Expected<ReceiveResult_t>{std::move(puback_packet)};
//                                    },
//                                    [&](SubscriptionAckPacket_t subback_packet) -> Expected<ReceiveResult_t> {
//                                        logger::debug(logger::AsyncMqttClient, "Received SubscribeAck packet: '{}'",
//                                                      detail::to_string(subback_packet));
//                                        return Expected<ReceiveResult_t>{std::move(subback_packet)};
//                                    },
//                                    []([[maybe_unused]] auto const& x) -> Expected<ReceiveResult_t> {
//                                        logger::warn(logger::AsyncMqttClient, "Unknown packet has been received");
//                                        return Unexpected{ErrorCode::UnknownPacket, "Unknown packet has been received"};
//                                    }
//                            }
//                    );
                } else {
                    co_return Unexpected{co_await handle_recv_error(packet.template get<async_mqtt::system_error>())};
                }
            }
    }

    boost::asio::awaitable<Error> async_connect()
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

      using boost::asio::ip::tcp;
      using boost::asio::redirect_error;
      using boost::asio::use_awaitable;

      {
        // Resolve name
        auto resolver = tcp::resolver{ep_.next_layer().get_executor()};
        auto res_ec = boost::system::error_code{};
        auto eps = co_await resolver.async_resolve(config_.host, config_.port, redirect_error(use_awaitable, res_ec));

        if (res_ec) {
          logger::err(logger::AsyncMqttClient, "Error while resolving name: '{}', errc: '{}'", res_ec.message(), res_ec.value());
          co_return Error { ErrorCode::NoService, "Error while resolving name of the broker" };
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
          co_return Error { detail::map_error_code(con_ec), "Error while establishing connection" };
        }
      }

      // Send ConnectPacket_t message
      if (auto system_error = co_await async_send_con(); system_error) {
        logger::err(logger::AsyncMqttClient, "Error while sending ConnectPacket: '{}'", system_error.what());
        co_return Error { detail::map_error_code(system_error.code()), "Error while sending ConnectPacket" };
      }

      // Receive con ack
      if (auto system_error = co_await async_recv_conack(); system_error) {
        logger::err(logger::AsyncMqttClient, "Error while receiving ConnectPacketAck error: '{}'", system_error.what());
        co_return Error{ detail::map_error_code(system_error.code()), "Error while receiving ConnectAckPacket" };
      }

      reconnect_.attempt = 0;

      co_return Error{};
    }

    boost::asio::awaitable<Expected<PacketId_t>> async_subscribe(std::vector<std::string> topics)
    {
      logger::trace(logger::AsyncMqttClient, "AsyncMqttClient::{}", __FUNCTION__);

        if (!connected()) {
            co_return Unexpected { ErrorCode::Disconnected, "Cannot subscribe when client is disconnected" };
        }

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

      if (!connected()) {
          co_return Unexpected { ErrorCode::Disconnected, "Cannot publish when client is disconnected" };
      }

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
            [&](const ConnectAckPacket_t& packet) {
                logger::debug(logger::AsyncMqttClient, "Received ConnectAckPacket packet: '{}'", detail::to_string(packet));
                connected_ = true;
            },
            [](const auto&) {
                logger::warn(logger::AsyncMqttClient, "Expected ConnectAckPacket packet, got some other packet");
            }
          });
        co_return async_mqtt::system_error{};
      } else {
        co_return co_await handle_recv_error(packet_variant.get<async_mqtt::system_error>());
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
    }

    boost::asio::awaitable<Error> handle_recv_error(const boost::system::system_error& error)
    {
        if (connected_)
        {
            if (co_await try_reconnect()) {
                co_return Error{ErrorCode::Reconnected, "Client got reconnected"};
            }
            else {
                co_return Error { ErrorCode::Disconnected, fmt::format("Client got disconnected {}",
                              reconnect_.attempt == reconnect_.max_attempts ? "after auto-reconnect failure" : "")};
            }
        }

        else {
            co_return Error {detail::map_error_code(error.code())};
        }
    }

    boost::asio::awaitable<bool> try_reconnect()
    {
        logger::trace(logger::AsyncMqttClient, "AsyncMqttEntityClient::{}", __FUNCTION__);

        if (reconnect_.max_attempts == 0) {
            co_return false;
        }

        reconnect_.attempt = 0;

        while (++reconnect_.attempt <= reconnect_.max_attempts) {
            auto error_code = boost::system::error_code{};

            logger::warn(logger::AsyncMqttClient, "AsyncMqttEntityClient auto reconnect, attempt: {}/{}", reconnect_.attempt, reconnect_.max_attempts);

            reconnect_.timer.expires_after(reconnect_.reconnect_delay);
            co_await reconnect_.timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, error_code));

            if (error_code) {
                co_return false;
            }

            auto error = co_await async_connect();

            if (not error) {
                co_return true;
            }
        }
        logger::err(logger::AsyncMqttClient, "AsyncMqttEntityClient auto-reconnect has failed after: {} attempts", reconnect_.max_attempts);

        co_return false;
    }

  private:
//    Executor executor_; // TODO(bielpa): Possibly can be removed
    ClientConfig config_;
    EndpointType ep_;
    std::optional<Will_t> will_ {std::nullopt};
    bool connected_ { false };
    Reconnect reconnect_ {};
  };
} // namespace mgmt::home_assistant::v2
