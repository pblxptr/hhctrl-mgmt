#include <catch2/catch_all.hpp>

#include <home_assistant/mqtt/sensor.hpp>

#include <boost/asio/co_spawn.hpp>
#include "tools.hpp"

using mgmt::home_assistant::mqtt::AsyncMqttClient;
using mgmt::home_assistant::mqtt::Sensor;
using mgmt::home_assistant::mqtt::SensorConfig;
using mgmt::home_assistant::mqtt::Qos_t;
using mgmt::home_assistant::mqtt::PublishPacket_t;
using mgmt::home_assistant::mqtt::PublishAckPacket_t;
using mgmt::home_assistant::mqtt::EntityConfig;

namespace {
    constexpr auto SensorUniqueId = "sensor_unique_id";
    const auto SensorDiscoveryTopic = fmt::format("{}/{}/{}/config", DefaultDiscoveryTopicPrefix, SensorConfig::EntityName, SensorUniqueId);

    auto sensor_client_config()
    {
        auto config = get_config();
        config.unique_id = SensorUniqueId;

        return config;
    }
}

TEST_CASE("Sensor entity can connect to a broker")
{
    auto ioc = IoContext();
    auto sensor = Sensor{SensorUniqueId, std::make_unique<AsyncMqttClient<>>(
            ioc.handle().get_executor(), sensor_client_config())
    };

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&sensor, &ioc]() -> boost::asio::awaitable<void> {
        const auto error_code = co_await sensor.async_connect();
        REQUIRE(!error_code);
        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

TEST_CASE("Sensor is configured properly")
{
    auto ioc = IoContext();
    using std::to_string;

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {

        // Arrange
        auto sensor = Sensor{SensorUniqueId, std::make_unique<AsyncMqttClient<>>(ioc.handle().get_executor(), sensor_client_config())};
        co_await async_connect(sensor);

        auto helper_client = AsyncMqttClient(ioc.handle().get_executor(), get_config());
        co_await async_connect(helper_client);
        co_await async_subscribe(helper_client, SensorDiscoveryTopic);

        SECTION("Sensor entity sends a config with required properties to a discovery topic")
        {
            // Act
            const auto configure_error = co_await sensor.async_configure(EntityConfig{});
            REQUIRE(!configure_error);

            // Assert
            const auto packet = co_await async_get_publish_packet(helper_client);
            const auto topic = static_cast<std::string_view>(packet.topic());
            REQUIRE(topic.data() == SensorDiscoveryTopic);

            const auto config = EntityConfig::from_json(to_string(packet.payload()));
            REQUIRE(config);
            REQUIRE(config->contains(SensorConfig::Property::StateTopic));

            SECTION("Sensor sends its sate")
            {
                // Arrange
                const auto state_topic = config->get(SensorConfig::Property::StateTopic);
                const auto state_value = std::string{"123"};
                REQUIRE(state_topic);
                co_await async_subscribe(helper_client, *state_topic);

                // Act
                const auto state_error = co_await sensor.async_set_state(state_value, Qos_t::at_most_once);

                // Assert
                REQUIRE(!state_error);

                const auto state_packet = co_await async_get_publish_packet(helper_client);
                REQUIRE(to_string(state_packet.payload()) == state_value);
            }
        }

        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}
