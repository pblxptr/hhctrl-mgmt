//
// Created by pp on 7/24/22.
//

#include <algorithm>

#include <home_assistant/event/temp_sensor_event_handler.hpp>
#include <home_assistant/event/logger.hpp>

namespace mgmt::home_assistant::event {
TempSensorEventHandler::TempSensorEventHandler(
  const adapter::EntityFactory& factory,
  const DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }
  , device_identity_provider_{ device_identity_provider }
{
  common::logger::get(Logger)->trace("TempSensorEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceCreated_t& event)
{
  common::logger::get(Logger)->trace("TempSensorEventHandler::{}(DeviceCreated)", __FUNCTION__);

  auto temp_sensor = co_await home_assistant::device::TempSensor::async_create(event.device_id, device_identity_provider_, factory_);
  if (!temp_sensor) {
      common::logger::get(Logger)->error("TempSensorEventHandler, cannot create TempSensor");

      co_return;
  }

  sensors_.push_back(std::move(*temp_sensor));

  auto executor = co_await boost::asio::this_coro::executor;

  boost::asio::co_spawn(executor, sensors_.back().async_run(), common::coro::rethrow);
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceRemoved_t& event)
{
  common::logger::get(Logger)->trace("TempSensorEventHandler::{}(DeviceRemoved)", __FUNCTION__);

  const auto device_id = event.device_id;
  const auto erased = std::erase_if(sensors_, [device_id](auto& sensor) {
    return device_id == sensor.hardware_id();
  });

  if (erased == 0) {
    common::logger::get(Logger)->error(fmt::format("TempSensor device with id: {} was not found", device_id));
  }

  co_return;
}

boost::asio::awaitable<void> TempSensorEventHandler::operator()([[maybe_unused]] const DeviceStateChanged_t& event)
{
  common::logger::get(Logger)->trace("TempSensorEventHandler::{}(DeviceStateChanged)", __FUNCTION__);

  const auto device_id = event.device_id;
  auto sensor = std::ranges::find_if(sensors_, [device_id](auto& sensor) {
    return device_id == sensor.hardware_id();
  });

  if (sensor == sensors_.end()) {
    common::logger::get(Logger)->error(fmt::format("TempSensor device with id: {} was not found", device_id));
  }

  co_await sensor->async_sync_state();
}
}// namespace mgmt::home_assistant::event
