#pragma once

#include <home_assistant/mqtt/entity_def.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <device/device_id.hpp>

namespace mgmt::home_assistant::device
{

class HatchHandler
{
public:
  HatchHandler(mgmt::device::DeviceId_t id, mgmt::home_assistant::mqttc::Cover_t cover)
    : id_{std::move(id)}
    , cover_{std::move(cover)}
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}, device id: {}", __FUNCTION__, id_);

    setup();
  }

  HatchHandler(const HatchHandler&) = delete;
  HatchHandler& operator=(const HatchHandler&) = delete;

  HatchHandler(HatchHandler&& rhs) noexcept
    : id_{std::move(rhs.id_)}
    , cover_{std::move(rhs.cover_)}
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::(HatchHandler&&)");

    setup();
  }

  HatchHandler& operator=(HatchHandler&& rhs)
  {
    id_ = std::move(rhs.id_);
    cover_ = std::move(rhs.cover_);

    setup();

    return *this;
  }

  ~HatchHandler() noexcept
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);
  }

  mgmt::device::DeviceId_t hardware_id() const
  {
    return id_;
  }

  void connect()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

    cover_.connect();
  }

  void async_sync_state()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

    const auto& hatch = mgmt::device::inventory<mgmt::device::Hatch_t>.get(id_);
    auto cover_state = mgmt::home_assistant::mqttc::CoverState{};

    //TODO: Rename status to state
    //TODO: Use mapper
    switch (hatch.status()) {
      case mgmt::device::HatchState::Open:
        cover_state = mgmt::home_assistant::mqttc::CoverState::Open;
        break;
      case mgmt::device::HatchState::Closed:
        cover_state = mgmt::home_assistant::mqttc::CoverState::Closed;
        break;
      default:
        spdlog::error("State not handled"); //TODO: Error just for the purpose of tests
    }

    cover_.async_set_state(cover_state);
  }

private:
  void setup()
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

    cover_.set_ack_handler([this]() { set_config(); });
    cover_.set_close_handler([this]() { on_close(); });
    cover_.set_error_handler([this]() { on_error(); });
    cover_.on_command([this](const auto& cmd) {
      handle_command(cmd);
    });
  }

  void handle_command(const mgmt::home_assistant::mqttc::CoverCommand& cmd)
  {
    common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

    auto& hatch = mgmt::device::inventory<mgmt::device::Hatch_t>.get(id_);

    switch (cmd) {
      case mgmt::home_assistant::mqttc::CoverCommand::Open:
        hatch.open();
        break;
      case mgmt::home_assistant::mqttc::CoverCommand::Close:
        hatch.close();
        break;
      case mgmt::home_assistant::mqttc::CoverCommand::Stop:
        spdlog::error("Command not supported"); //TODO: Error just for the purpose of tests
        break;
    }
  }

private:
    void set_config()
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

      auto config = mgmt::home_assistant::mqttc::EntityConfig{cover_.unique_id()};
      config.set("device_class", "door");
      cover_.async_set_config(std::move(config));
      cover_.async_set_availibility(mgmt::home_assistant::mqttc::Availibility::Online);

      async_sync_state();
    }

    void on_close()
    {
      spdlog::error("HatchHandler::{}", __FUNCTION__);
    }

    void on_error()
    {
      spdlog::error("HatchHandler::{}", __FUNCTION__);
    }

private:
  mgmt::device::DeviceId_t id_;
  mgmt::home_assistant::mqttc::Cover_t cover_;
};
}
