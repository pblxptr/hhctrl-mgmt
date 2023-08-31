#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <home_assistant/mqtt/coverv2.hpp>

#include <boost/asio/co_spawn.hpp>
#include "tools.hpp"

using mgmt::home_assistant::v2::AsyncMqttClient;
using mgmt::home_assistant::v2::Cover;
using mgmt::home_assistant::v2::CoverConfig;
using mgmt::home_assistant::v2::Qos_t;
using mgmt::home_assistant::v2::PublishPacket_t;
using mgmt::home_assistant::v2::PublishAckPacket_t;
using mgmt::home_assistant::v2::SubscriptionAckPacket_t;
using mgmt::home_assistant::v2::EntityConfig;
using mgmt::home_assistant::v2::ProtocolVersion_t;

namespace {
    constexpr auto CoverUniqueId = "cover_unique_id";
    const auto CoverDiscoveryTopic = fmt::format("{}/{}/{}/config", DefaultDiscoveryTopicPrefix, CoverConfig::EntityName, CoverUniqueId);

    auto cover_client_config()
    {
        auto config = get_config();
        config.unique_id = CoverUniqueId;

        return config;
    }
}

TEST_CASE("Cover entity can connect to a broker")
{
    auto ioc = IoContext();
    auto cover = Cover{CoverUniqueId, AsyncMqttClient{
        ioc.handle().get_executor(), cover_client_config()}
    };

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&cover, &ioc]() -> boost::asio::awaitable<void> {
        const auto error_code = co_await cover.async_connect();
        REQUIRE(!error_code);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

template <typename Executor, ProtocolVersion_t protocolVersion>
boost::asio::awaitable<PublishPacket_t> async_get_publish_packet(AsyncMqttClient<Executor, protocolVersion>& client)
{
    const auto& result = co_await client.async_receive();
    REQUIRE(result);
    const auto& value = result.value();
    REQUIRE(std::holds_alternative<PublishPacket_t>(value));

    co_return std::get<PublishPacket_t>(value);
}

template <typename Executor, ProtocolVersion_t protocolVersion>
boost::asio::awaitable<void> async_setup(AsyncMqttClient<Executor, protocolVersion>& client)
{
    const auto error_code = co_await client.async_connect();
    REQUIRE(!error_code);

    {
        auto sub_topics = std::vector<std::string>{CoverDiscoveryTopic};
        const auto result = co_await client.async_subscribe(std::move(sub_topics));
        REQUIRE(result);
    }

    {
        const auto result = co_await client.async_receive();
        REQUIRE(result);
        REQUIRE(std::holds_alternative<SubscriptionAckPacket_t>(result.value()));
    }
}

template <typename Executor, ProtocolVersion_t protocolVersion>
boost::asio::awaitable<void> async_setup(Cover<AsyncMqttClient<Executor, protocolVersion>>& cover)
{
    const auto error_code = co_await cover.async_connect();
    REQUIRE(!error_code);
}

TEST_CASE("Cover is configured properly")
{
    auto ioc = IoContext();
    using std::to_string;

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
        auto cover = Cover{CoverUniqueId, AsyncMqttClient{ioc.handle().get_executor(), cover_client_config()}};
        co_await async_setup(cover);

        auto helper_client = AsyncMqttClient(ioc.handle().get_executor(), get_config());
        co_await async_setup(helper_client);

        SECTION("Cover entity sends a config with required properties to a discovery topic")
        {
            // Send config
            const auto configure_error = co_await cover.async_configure(EntityConfig{});
            REQUIRE(!configure_error);

            // Verify config by helper client
            const auto packet = co_await async_get_publish_packet(helper_client);
            const auto topic = static_cast<std::string_view>(packet.topic());
            REQUIRE(topic.data() == CoverDiscoveryTopic);

            const auto config = EntityConfig::from_json(to_string(packet.payload()));
            REQUIRE(config);
            REQUIRE(config->contains(CoverConfig::Property::StateTopic));
            REQUIRE(config->contains(CoverConfig::Property::SwitchCommandTopic));

            SECTION("Cover receives a valid switch open command")
            {
                using mgmt::home_assistant::v2::CoverSwitchCommand;

                const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                const auto switch_command_value = config->get(CoverConfig::Property::PayloadOpen);
                REQUIRE(switch_command_topic);
                REQUIRE(switch_command_topic);

                const auto publish_result = co_await helper_client.async_publish(*switch_command_topic, *switch_command_value, Qos_t::at_most_once);
                REQUIRE(publish_result);

                const auto command = co_await cover.async_receive();
                REQUIRE(command);
                REQUIRE(std::holds_alternative<CoverSwitchCommand>(command.value()));
                REQUIRE(std::get<CoverSwitchCommand>(command.value()) == CoverSwitchCommand::Open);
            }

            SECTION("Cover receives a valid switch close command")
            {
                using mgmt::home_assistant::v2::CoverSwitchCommand;

                const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                const auto switch_command_value = config->get(CoverConfig::Property::PayloadClose);
                REQUIRE(switch_command_topic);
                REQUIRE(switch_command_topic);

                const auto publish_result = co_await helper_client.async_publish(*switch_command_topic, *switch_command_value, Qos_t::at_most_once);
                REQUIRE(publish_result);

                const auto command = co_await cover.async_receive();
                REQUIRE(command);
                REQUIRE(std::holds_alternative<CoverSwitchCommand>(command.value()));
                REQUIRE(std::get<CoverSwitchCommand>(command.value()) == CoverSwitchCommand::Close);
            }
        }

        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run(std::chrono::seconds{5});
}

