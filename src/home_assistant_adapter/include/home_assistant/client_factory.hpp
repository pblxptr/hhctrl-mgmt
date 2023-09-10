#pragma once

#include <boost/asio/io_context.hpp>
#include <home_assistant/adapter/broker_config.hpp>
#include <home_assistant/mqtt/async_mqtt_client.hpp>

namespace mgmt::home_assistant::adapter
{
    class MqttClientFactory
    {
    public:
        MqttClientFactory(boost::asio::io_context& ioc, BrokerConfig config)
            : ioc_{ioc}
            , config_{std::move(config)}
        {}

        [[nodiscard]] auto create_client(const std::string& unique_id) const
        {
            auto executor = ioc_.get_executor();

            return v2::AsyncMqttClient<v2::ProtocolVersion_t::v3_1_1> {
                std::move(executor),
                v2::ClientConfig {
                    .unique_id = unique_id,
                    .username = config_.username,
                    .password = config_.password,
                    .host = config_.host,
                    .port = config_.port,
                    .clean_session = true,
                    .keep_alive = 15
                }
            };
        }

    private:
        boost::asio::io_context& ioc_;
        BrokerConfig config_;
    };
}
