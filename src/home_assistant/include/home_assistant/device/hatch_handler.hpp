#pragma once

#include <utility>

#include <home_assistant/mqtt/entity_def.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/mqtt/entity_error.hpp>
#include <home_assistant/mqtt/cover.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/device_identity.hpp>
#include <home_assistant/unique_id.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::device {

class HatchHandler
{
public:
  HatchHandler(
    mgmt::device::DeviceId_t device_id,
    const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
    const mgmt::home_assistant::EntityFactory& factory);

  // movable
  HatchHandler(HatchHandler&& rhs) noexcept;
  HatchHandler& operator=(HatchHandler&& rhs) noexcept;
  // non-copyable
  HatchHandler(const HatchHandler&) = delete;
  HatchHandler& operator=(const HatchHandler&) = delete;

  ~HatchHandler() = default;

  mgmt::device::DeviceId_t hardware_id() const;
  boost::asio::awaitable<void> async_connect();
  boost::asio::awaitable<void> async_sync_state();

private:
  void setup();
  boost::asio::awaitable<void> async_set_config();
  void handle_command(const mgmt::home_assistant::mqttc::CoverCommand& cmd) const;
  void on_error(const mgmt::home_assistant::mqttc::EntityError& error);

private:
  mgmt::device::DeviceId_t device_id_;
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider_;
  mgmt::home_assistant::mqttc::Cover_t cover_;
};
}// namespace mgmt::home_assistant::device
