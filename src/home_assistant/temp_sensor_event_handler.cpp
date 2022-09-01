//
// Created by pp on 7/24/22.
//

#include <algorithm>

#include <home_assistant/device/temp_sensor_event_handler.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::device {
TempSensorEventHandler::TempSensorEventHandler(
  const EntityFactory& factory,
  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }
  , device_identity_provider_{ device_identity_provider }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceCreated_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorEventHandler::{}(DeviceCreated)", __FUNCTION__);

  sensors_.emplace_back(event.device_id, device_identity_provider_, factory_);

  auto& sensor = sensors_.back();
  co_await sensor.async_connect();
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceRemoved_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorEventHandler::{}(DeviceRemoved)", __FUNCTION__);

  const auto device_id = event.device_id;
  const auto erased = std::erase_if(sensors_, [device_id](auto& h) {
    return device_id == h.hardware_id();
  });

  if (not erased) {
    throw std::runtime_error(fmt::format("TempSensor device with id: {} was not found", device_id));
  }

  co_return;
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceStateChanged_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorEventHandler::{}(DeviceStateChanged)", __FUNCTION__);

  const auto device_id = event.device_id;
  auto sensor = std::ranges::find_if(sensors_, [device_id](auto& h) {
    return device_id == h.hardware_id();
  });

  if (sensor == sensors_.end()) {
    throw std::runtime_error(fmt::format("TempSensor device with id: {} was not found", device_id));
  }

  co_await sensor->async_sync_state();
}
void TempSensorEventHandler::on_error()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorEventHandler::{}", __FUNCTION__);
}
}// namespace mgmt::home_assistant::device
