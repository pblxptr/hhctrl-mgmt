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

namespace {
    const auto CoverDiscoveryTopic = fmt::format("{}/{}/{}/config", DefaultDiscoveryTopicPrefix, CoverConfig::EntityName, DefaultUniqueId);
    constexpr auto CoverUniqueId = "cover_unique_id";

    auto cover_config()
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
        ioc.handle().get_executor(), cover_config()}
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

template <typename Executor>
boost::asio::awaitable<AsyncMqttClient<Executor>> async_create_and_prepare_helper_client(Executor executor)
{
    auto client = AsyncMqttClient(executor, get_config());

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

    co_return client;
}

template <typename Executor>
boost::asio::awaitable<Cover<AsyncMqttClient<Executor>>> async_create_and_prepare_cover(Executor executor)
{
    auto cover = Cover{CoverUniqueId, AsyncMqttClient{executor, cover_config()}};

    const auto error_code = co_await cover.async_connect();
    REQUIRE(!error_code);

    co_return cover;
}

TEST_CASE("Cover is configured properly")
{
    auto ioc = IoContext();

    // NOLINTBEGIN
    boost::asio::co_spawn(ioc.handle(), [&ioc]() -> boost::asio::awaitable<void> {
        auto cover = co_await async_create_and_prepare_cover(ioc.handle().get_executor());
        auto helper_client = co_await async_create_and_prepare_helper_client(ioc.handle().get_executor());

        SECTION("Cover entity sends a valid config to discovery topic")
        {

        }














        ioc.stop();

    }, rethrow);
    // NOLINTEND

    ioc.run();
}

