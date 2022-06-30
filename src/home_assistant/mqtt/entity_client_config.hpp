#pragma once

#include <boost/asio.hpp>
#include <string>
#include <mqtt/client.hpp>
#include <home_assistant/mqtt/entity_client.hpp>

namespace mgmt::home_assistant::mqttc
{
  template<class... Args>
  auto make_mqtt_client(Args&&... args)
  {
    return mqtt::make_client(args...);
  }

  using Client_t = decltype(make_mqtt_client(
    std::declval<boost::asio::io_context>(),
    std::declval<std::string>(),
    std::declval<std::uint16_t>()
  ));

  template<class Impl>
  auto make_entity_client(std::string unique_id, Impl impl)
  {
    return home_assistant::mqttc::MqttEntityClient{std::move(unique_id), std::move(impl)};
  }

  using EntityClient_t = decltype(make_entity_client(
    std::declval<std::string>(),
    std::declval<Client_t>()
  ));
}