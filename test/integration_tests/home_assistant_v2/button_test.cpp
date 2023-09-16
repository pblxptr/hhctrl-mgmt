#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <home_assistant/mqtt/button.hpp>

#include <boost/asio/co_spawn.hpp>
#include "tools.hpp"

using mgmt::home_assistant::mqtt::AsyncMqttClient;
using mgmt::home_assistant::mqtt::Button;
using mgmt::home_assistant::mqtt::ButtonConfig;
using mgmt::home_assistant::mqtt::Qos_t;
using mgmt::home_assistant::mqtt::PublishPacket_t;
using mgmt::home_assistant::mqtt::PublishAckPacket_t;
using mgmt::home_assistant::mqtt::SubscriptionAckPacket_t;
using mgmt::home_assistant::mqtt::EntityConfig;
using mgmt::home_assistant::mqtt::ProtocolVersion_t;

namespace {
    constexpr auto ButtonUniqueId = "button_unique_id";
    const auto ButtonDiscoveryTopic = fmt::format("{}/{}/{}/config", DefaultDiscoveryTopicPrefix, ButtonConfig::EntityName, ButtonUniqueId);

    auto button_client_config()
    {
        auto config = get_config();
        config.unique_id = ButtonUniqueId;

        return config;
    }
}

TEST_CASE("Button entity can connect to a broker")
{
    auto ioc = IoContext();
    auto button = Button{ButtonUniqueId, std::make_unique<AsyncMqttClient<>>(
        ioc.handle().get_executor(), button_client_config())
    };

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&button, &ioc]() -> boost::asio::awaitable<void> {
        const auto error_code = co_await button.async_connect();
        REQUIRE(!error_code);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

TEST_CASE("Button is configured properly")
{
    auto ioc = IoContext();
    using std::to_string;

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {

        // Arrange
        auto button = Button{ButtonUniqueId, std::make_unique<AsyncMqttClient<>>(ioc.handle().get_executor(), button_client_config())};
        co_await async_connect(button);

        auto helper_client = AsyncMqttClient(ioc.handle().get_executor(), get_config());
        co_await async_connect(helper_client);
        co_await async_subscribe(helper_client, ButtonDiscoveryTopic);

        SECTION("Button entity sends a config with required properties to a discovery topic")
        {
            // Act
            const auto configure_error = co_await button.async_configure(EntityConfig{});
            REQUIRE(!configure_error);

            // Assert
            const auto packet = co_await async_get_publish_packet(helper_client);
            const auto topic = static_cast<std::string_view>(packet.topic());
            REQUIRE(topic.data() == ButtonDiscoveryTopic);

            const auto config = EntityConfig::from_json(to_string(packet.payload()));
            REQUIRE(config);
            REQUIRE(config->contains(ButtonConfig::Property::CommandTopic));

            SECTION("Button handles command")
            {
                SECTION("Button receives a valid press command")
                {
                    // Arrange
                    const auto press_command_topic = config->get(ButtonConfig::Property::CommandTopic);
                    const auto press_command_value = config->get(ButtonConfig::Property::PayloadPress);
                    REQUIRE(press_command_topic);
                    REQUIRE(press_command_value);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*press_command_topic,
                                                                                     *press_command_value,
                                                                                     Qos_t::at_most_once);
                    REQUIRE(publish_result);
                    const auto command = co_await button.async_receive();

                    // Assert
                    REQUIRE(command);
                }

                SECTION("When button receives invalid payload for a command topic it returns appropriate error code")
                {
                    using mgmt::home_assistant::mqtt::ErrorCode;

                    // Arrange
                    const auto press_command_topic = config->get(ButtonConfig::Property::CommandTopic);
                    const auto press_command_value = config->get(ButtonConfig::Property::PayloadPress);
                    REQUIRE(press_command_topic);
                    REQUIRE(press_command_value);

                    // Act
                    const auto publish_result = co_await helper_client.async_publish(*press_command_topic,
                                                                                     *press_command_value + "invalid_payload",
                                                                                     Qos_t::at_most_once);
                    const auto command = co_await button.async_receive();

                    // Assert
                    REQUIRE(publish_result);
                    REQUIRE(!command);
                    REQUIRE(command.error().code() == ErrorCode::UnknownPacket);
                }
            }
        }


        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

