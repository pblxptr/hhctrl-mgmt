#pragma once

#include <boost/asio/io_context.hpp>
#include <spdlog/spdlog.h>

#include <home_assistant/mqtt/entity_client_config.hpp>
#include <home_assistant/mqtt/cover.hpp>
#include <home_assistant/entity_store.hpp>
#include <home_assistant/mqtt/entity_client_factory.hpp>

namespace mgmt::home_assistant
{
class HatchDeviceHandler
{
  using EntityClient_t = decltype(std::declval<mgmt::home_assistant::mqttc::EntityClientFactory>().create(
    std::declval<std::string>()
  ));
  using CoverEntity_t = mgmt::home_assistant::mqttc::Cover<EntityClient_t>;

public:
  HatchDeviceHandler(mgmt::home_assistant::mqttc::EntityClientFactory& factory)
    : factory_{factory}
  {}

  void add_device(const std::string& unique_id)
  {
    auto client = factory_.create(unique_id);
    client.set_error_handler([this](auto&& ec) { on_error(ec); });
    client.set_connack_handler([this, unique_id](auto&& sp, auto&& ec) {
      spdlog::debug("HatchDeviceHandler::{}::set_connack_handler", __FUNCTION__);

      configure_device(unique_id);
      return true;
    });

    auto entity = CoverEntity_t{unique_id, std::move(client)};
    entity.async_connect();

    entity_store<CoverEntity_t>.add(std::move(entity));
  }

  void remove_device(const std::string& unique_id)
  {
    spdlog::debug("HatchDeviceHandler::{}", __FUNCTION__);

    entity_store<CoverEntity_t>.remove(unique_id);
  }

  void change_device_state(const std::string& unique_id)
  {
    spdlog::debug("HatchDeviceHandler::{}", __FUNCTION__);

    auto& entity = entity_store<CoverEntity_t>.get(unique_id);
    entity.async_set_state(mgmt::home_assistant::mqttc::CoverState::Closed);
  }
private:
  void configure_device(const std::string& unique_id)
  {
    auto config = mgmt::home_assistant::mqttc::EntityConfig{unique_id};
    config.set("name", "My hatch device");
    config.set("device_class", "door");

    auto& entity = entity_store<CoverEntity_t>.get(unique_id);
    entity.on_command([](auto&& cmd) {
      spdlog::debug("Dupa");
     });
    entity.async_set_config(std::move(config));
    entity.async_set_availibility(mgmt::home_assistant::mqttc::Availibility::Online);
    entity.async_set_state(mgmt::home_assistant::mqttc::CoverState::Open);
  }

  void on_error(const boost::system::error_code& ec)
  {
    spdlog::debug("HatchDeviceHandler::{}", __FUNCTION__);
  }
private:
  mgmt::home_assistant::mqttc::EntityClientFactory& factory_;
};
}
