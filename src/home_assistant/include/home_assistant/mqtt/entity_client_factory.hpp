#pragma once

#include <string>
#include <boost/asio/io_context.hpp>

#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/mqtt/async_entity_client.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::mqttc {
class EntityClientFactory
{
public:
  EntityClientFactory(
    boost::asio::io_context& ioc,
    mgmt::home_assistant::mqttc::EntityClientConfig config
    )
    : ioc_{ ioc }
    , config_{ std::move(config) }
  {}

  auto create(std::string unique_id) const
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("Creating client for unique_id: {}, server: {}, port: {}",
      unique_id, config_.server_address, config_.server_port);

    return MqttEntityClient{ std::move(unique_id),
      mqtt::make_client(ioc_, config_.server_address, config_.server_port), config_ };
  }

  auto create_async_client(std::string unique_id)
  {
    return AsyncMqttEntityClient{ std::move(unique_id),
      mqtt::make_async_client(ioc_, config_.server_address, config_.server_port), config_ };
  }


private:
  boost::asio::io_context& ioc_;
  mgmt::home_assistant::mqttc::EntityClientConfig config_;
};
}// namespace mgmt::home_assistant::mqttc