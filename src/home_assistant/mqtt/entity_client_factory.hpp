#pragma once

#include <string>
#include <boost/asio/io_context.hpp>
#include <mqtt/client.hpp>
#include <home_assistant/mqtt/entity_client.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::mqttc
{
  class EntityClientFactory
  {
  public:
    EntityClientFactory(
      boost::asio::io_context& ioc,
      std::string server_address,
      std::uint16_t server_port
    )
      : ioc_{ioc}
      , server_address_{ std::move(server_address) }
      , server_port_{ server_port }
    {}

    auto create(std::string unique_id) const
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("Creating client for unique_id: {}, server: {}, port: {}",
        unique_id, server_address_, server_port_
      );

      return MqttEntityClient { std::move(unique_id),
        mqtt::make_client(ioc_, server_address_, server_port_)
      };
    }
  private:
    boost::asio::io_context& ioc_;
    std::string server_address_;
    std::uint16_t server_port_;
  };
}