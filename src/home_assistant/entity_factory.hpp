#pragma once

#include <spdlog/spdlog.h>

#include <home_assistant/mqtt/entity_client_factory.hpp>
#include <home_assistant/mqtt/cover.hpp>

namespace mgmt::home_assistant
{
  class EntityFactory
  {
  public:
    EntityFactory(const mgmt::home_assistant::mqttc::EntityClientFactory& client_factory)
      : client_factory_{client_factory}
    {}

    auto create_cover(std::string unique_id) const
    {
      using EntityClient_t = decltype(client_factory_.create(std::declval<std::string>()));

      return mgmt::home_assistant::mqttc::Cover<EntityClient_t>(unique_id, client_factory_.create(unique_id));
    }

  private:
    const mgmt::home_assistant::mqttc::EntityClientFactory& client_factory_;
  };
}


