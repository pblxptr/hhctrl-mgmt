#pragma once

#include <utility>

#include <home_assistant/mqtt/entity_def.hpp>
#include <home_assistant/mqtt/entity_config.hpp>
#include <home_assistant/mqtt/entity_error.hpp>
#include <home_assistant/mqtt/sensor.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/device_identity.hpp>
#include <home_assistant/unique_id.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::device {

class TempSensorHandler
{
public:
  TempSensorHandler(
    mgmt::device::DeviceId_t device_id,
    const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
    const mgmt::home_assistant::EntityFactory& factory);

  TempSensorHandler(const TempSensorHandler&) = delete;
  TempSensorHandler& operator=(const TempSensorHandler&) = delete;
  TempSensorHandler(TempSensorHandler&& rhs) noexcept;
  TempSensorHandler& operator=(TempSensorHandler&& rhs) noexcept;

  mgmt::device::DeviceId_t hardware_id() const;
  boost::asio::awaitable<void> async_connect();
  boost::asio::awaitable<void> async_sync_state();

private:
  void setup();
  boost::asio::awaitable<void> async_set_config();
  void on_error(const mgmt::home_assistant::mqttc::EntityError& error);

private:
  mgmt::device::DeviceId_t device_id_;
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider_;
  mgmt::home_assistant::mqttc::Sensor_t sensor_;
};
}// namespace mgmt::home_assistant::device
