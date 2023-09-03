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
using mgmt::home_assistant::v2::CoverState;

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

TEST_CASE("Cover is configured properly")
{
    auto ioc = IoContext();
    using std::to_string;

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {

        // Arrange
        auto cover = Cover{CoverUniqueId, AsyncMqttClient{ioc.handle().get_executor(), cover_client_config()}};
        co_await async_connect(cover);

        auto helper_client = AsyncMqttClient(ioc.handle().get_executor(), get_config());
        co_await async_connect(helper_client);
        co_await async_subscribe(helper_client, CoverDiscoveryTopic);

        SECTION("Cover entity sends a config with required properties to a discovery topic")
        {
            // Act
            const auto configure_error = co_await cover.async_configure(EntityConfig{});
            REQUIRE(!configure_error);

            // Assert
            const auto packet = co_await async_get_publish_packet(helper_client);
            const auto topic = static_cast<std::string_view>(packet.topic());
            REQUIRE(topic.data() == CoverDiscoveryTopic);

            const auto config = EntityConfig::from_json(to_string(packet.payload()));
            REQUIRE(config);
            REQUIRE(config->contains(CoverConfig::Property::StateTopic));
            REQUIRE(config->contains(CoverConfig::Property::SwitchCommandTopic));

            SECTION("Cover handles command")
            {
                SECTION("Cover receives a valid switch open command")
                {
                    // Arrange
                    using mgmt::home_assistant::v2::CoverSwitchCommand;

                    const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                    const auto switch_command_value = config->get(CoverConfig::Property::PayloadOpen);
                    REQUIRE(switch_command_topic);
                    REQUIRE(switch_command_topic);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*switch_command_topic,
                                                                                     *switch_command_value,
                                                                                     Qos_t::at_most_once);
                    REQUIRE(publish_result);
                    const auto command = co_await cover.async_receive();

                    // Assert
                    REQUIRE(command);
                    REQUIRE(std::holds_alternative<CoverSwitchCommand>(command.value()));
                    REQUIRE(std::get<CoverSwitchCommand>(command.value()) == CoverSwitchCommand::Open);
                }

                SECTION("Cover receives a valid switch close command")
                {
                    // Arrange
                    using mgmt::home_assistant::v2::CoverSwitchCommand;

                    const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                    const auto switch_command_value = config->get(CoverConfig::Property::PayloadClose);
                    REQUIRE(switch_command_topic);
                    REQUIRE(switch_command_topic);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*switch_command_topic,
                                                                                     *switch_command_value,
                                                                                     Qos_t::at_most_once);
                    REQUIRE(publish_result);
                    const auto command = co_await cover.async_receive();

                    // Assert
                    REQUIRE(command);
                    REQUIRE(std::holds_alternative<CoverSwitchCommand>(command.value()));
                    REQUIRE(std::get<CoverSwitchCommand>(command.value()) == CoverSwitchCommand::Close);
                }

                SECTION("Cover receives a valid switch stop command")
                {
                    // Arrange
                    using mgmt::home_assistant::v2::CoverSwitchCommand;

                    const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                    const auto switch_command_value = config->get(CoverConfig::Property::PayloadStop);
                    REQUIRE(switch_command_topic);
                    REQUIRE(switch_command_topic);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*switch_command_topic,
                                                                                     *switch_command_value,
                                                                                     Qos_t::at_most_once);
                    REQUIRE(publish_result);
                    const auto command = co_await cover.async_receive();

                    // Assert
                    REQUIRE(command);
                    REQUIRE(std::holds_alternative<CoverSwitchCommand>(command.value()));
                    REQUIRE(std::get<CoverSwitchCommand>(command.value()) == CoverSwitchCommand::Stop);
                }

                SECTION("Cover throws and exception  when an invalid packet with invalid payload is submitted to a command topic")
                {
                    // Arrange
                    using mgmt::home_assistant::v2::CoverSwitchCommand;
                    using mgmt::home_assistant::v2::ErrorCode;

                    const auto switch_command_topic = config->get(CoverConfig::Property::SwitchCommandTopic);
                    const auto switch_command_value = config->get(CoverConfig::Property::PayloadOpen);
                    REQUIRE(switch_command_topic);
                    REQUIRE(switch_command_topic);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*switch_command_topic,
                                                                                     *switch_command_value + "invalid_payload",
                                                                                     Qos_t::at_most_once);
                    // Assert
                    REQUIRE(publish_result);
                    REQUIRE_THROWS(co_await cover.async_receive());
                }
            }

            SECTION("Cover sends its sate")
            {
                SECTION("Cover sends 'open' state")
                {
                    // Arrange
                    const auto state_topic = config->get(CoverConfig::Property::StateTopic);
                    const auto state_value = config->get(CoverConfig::Property::StateOpen);
                    REQUIRE(state_topic);
                    REQUIRE(state_value);
                    co_await async_subscribe(helper_client, *state_topic);

                    // Act
                    const auto state_error = co_await cover.async_set_state(CoverState::Open, Qos_t::at_most_once);

                    // Assert
                    REQUIRE(!state_error);

                    const auto state_packet = co_await async_get_publish_packet(helper_client);
                    REQUIRE(to_string(state_packet.payload()) == *state_value);
                }

                SECTION("Cover sends 'closed' state")
                {
                    // Arrange
                    const auto state_topic = config->get(CoverConfig::Property::StateTopic);
                    const auto state_value = config->get(CoverConfig::Property::StateClosed);
                    REQUIRE(state_topic);
                    REQUIRE(state_value);
                    co_await async_subscribe(helper_client, *state_topic);

                    // Act
                    const auto state_error = co_await cover.async_set_state(CoverState::Closed, Qos_t::at_most_once);

                    // Assert
                    REQUIRE(!state_error);

                    const auto state_packet = co_await async_get_publish_packet(helper_client);
                    REQUIRE(to_string(state_packet.payload()) == *state_value);
                }
            }
        }


        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

