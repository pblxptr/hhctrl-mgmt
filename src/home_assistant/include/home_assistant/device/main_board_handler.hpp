#pragma once

#include <logger/logger.hpp>
#include <device/device_id.hpp>
#include <device/indicator.hpp>
#include <home_assistant/mqtt/entity_def.hpp>
#include <home_assistant/entity_factory_legacy.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/logger.hpp>
#include <home_assistant/mqtt/entity_error.hpp>

namespace mgmt::home_assistant::device {
class MainBoardHandler
{
public:
  MainBoardHandler(
    mgmt::device::DeviceId_t device_id,
    const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
    const mgmt::home_assistant::EntityFactory& factory);

  // movable
  MainBoardHandler(MainBoardHandler&& rhs) noexcept;
  MainBoardHandler& operator=(MainBoardHandler&& rhs) noexcept;
  // non-copyable
  MainBoardHandler(const MainBoardHandler&) = delete;
  MainBoardHandler& operator=(const MainBoardHandler&) = delete;

  ~MainBoardHandler() = default;

  mgmt::device::DeviceId_t hardware_id() const;
  boost::asio::awaitable<void> async_connect();
  boost::asio::awaitable<void> async_sync_state();

private:
  void setup();
  boost::asio::awaitable<void> async_set_config_indicator(const mgmt::device::IndicatorType& type);
  boost::asio::awaitable<void> async_set_config_restart_button();
  void on_error(const mgmt::home_assistant::mqttc::EntityError& error);

private:
  mgmt::device::DeviceId_t device_id_;
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider_;
  std::unordered_map<mgmt::device::IndicatorType,
    mgmt::home_assistant::mqttc::BinarySensor_t>
    indicators_;
  mgmt::home_assistant::mqttc::Button_t restart_button_;
};
}// namespace mgmt::home_assistant::device
