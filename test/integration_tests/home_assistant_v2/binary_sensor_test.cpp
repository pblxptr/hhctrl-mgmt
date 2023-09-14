#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/async_mqtt_client.hpp>
#include <home_assistant/mqtt/binary_sensor2.hpp>

#include <boost/asio/co_spawn.hpp>
#include "tools.hpp"

using mgmt::home_assistant::v2::AsyncMqttClient;
using mgmt::home_assistant::v2::BinarySensor;
using mgmt::home_assistant::v2::BinarySensorConfig;
using mgmt::home_assistant::v2::Qos_t;
using mgmt::home_assistant::v2::PublishPacket_t;
using mgmt::home_assistant::v2::PublishAckPacket_t;
using mgmt::home_assistant::v2::EntityConfig;
using mgmt::home_assistant::v2::BinarySensorState;


namespace {
    constexpr auto BinarySensorUniqueId = "binary_sensor_unique_id";
    const auto BinarySensorDiscoveryTopic = fmt::format("{}/{}/{}/config", DefaultDiscoveryTopicPrefix, BinarySensorConfig::EntityName, BinarySensorUniqueId);

    auto binary_sensor_client_config()
    {
        auto config = get_config();
        config.unique_id = BinarySensorUniqueId;

        return config;
    }
}

TEST_CASE("Binary sensor entity can connect to a broker")
{
    auto ioc = IoContext();
    auto binary_sensor = BinarySensor{BinarySensorUniqueId, std::make_unique<AsyncMqttClient<>>(
            ioc.handle().get_executor(), binary_sensor_client_config())
    };

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&binary_sensor, &ioc]() -> boost::asio::awaitable<void> {
        const auto error_code = co_await binary_sensor.async_connect();
        REQUIRE(!error_code);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

TEST_CASE("BinarySensor is configured properly")
{
    auto ioc = IoContext();
    using std::to_string;

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {

        // Arrange
        auto binary_sensor = BinarySensor{BinarySensorUniqueId, std::make_unique<AsyncMqttClient<>>(ioc.handle().get_executor(), binary_sensor_client_config())};
        co_await async_connect(binary_sensor);

        auto helper_client = AsyncMqttClient(ioc.handle().get_executor(), get_config());
        co_await async_connect(helper_client);
        co_await async_subscribe(helper_client, BinarySensorDiscoveryTopic);

        SECTION("BinarySensor entity sends a config with required properties to a discovery topic")
        {
            // Act
            const auto configure_error = co_await binary_sensor.async_configure(EntityConfig{});
            REQUIRE(!configure_error);

            // Assert
            const auto packet = co_await async_get_publish_packet(helper_client);
            const auto topic = static_cast<std::string_view>(packet.topic());
            REQUIRE(topic.data() == BinarySensorDiscoveryTopic);

            const auto config = EntityConfig::from_json(to_string(packet.payload()));
            REQUIRE(config);
            REQUIRE(config->contains(BinarySensorConfig::Property::StateTopic));

            SECTION("BinarySensor sends its sate")
            {
                SECTION("BinarySensor sends 'on' state")
                {
                    // Arrange
                    const auto state_topic = config->get(BinarySensorConfig::Property::StateTopic);
                    const auto state_value = config->get(BinarySensorConfig::Property::StateOn);
                    REQUIRE(state_topic);
                    REQUIRE(state_value);
                    co_await async_subscribe(helper_client, *state_topic);

                    // Act
                    const auto state_error = co_await binary_sensor.async_set_state(BinarySensorState::On, Qos_t::at_most_once);

                    // Assert
                    REQUIRE(!state_error);

                    const auto state_packet = co_await async_get_publish_packet(helper_client);
                    REQUIRE(to_string(state_packet.payload()) == *state_value);
                }

                SECTION("BinarySensor sends 'off' state")
                {
                    // Arrange
                    const auto state_topic = config->get(BinarySensorConfig::Property::StateTopic);
                    const auto state_value = config->get(BinarySensorConfig::Property::StateOff);
                    REQUIRE(state_topic);
                    REQUIRE(state_value);
                    co_await async_subscribe(helper_client, *state_topic);

                    // Act
                    const auto state_error = co_await binary_sensor.async_set_state(BinarySensorState::Off, Qos_t::at_most_once);

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
