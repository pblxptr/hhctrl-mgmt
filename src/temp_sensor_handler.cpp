//
// Created by pp on 7/24/22.
//

#include <home_assistant/device/temp_sensor_handler.hpp>
#include <device/temp_sensor_t.hpp>

namespace mgmt::home_assistant::device {
TempSensorHandler::TempSensorHandler(
  mgmt::device::DeviceId_t device_id,
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
  const mgmt::home_assistant::EntityFactory& factory)
  : device_id_{ std::move(device_id) }, identity_provider_{ identity_provider }
  , sensor_{ factory.create_sensor(get_unique_id(device_id_, identity_provider_.identity(device_id_))) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::{}, device id: {}", __FUNCTION__, device_id_);

  setup();
}

TempSensorHandler::TempSensorHandler(TempSensorHandler&& rhs) noexcept
  : device_id_{ std::move(rhs.device_id_) }
  , identity_provider_{ rhs.identity_provider_ }
  , sensor_{ std::move(rhs.sensor_) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::(TempSensorHandler&&)");

  setup();
}

TempSensorHandler& TempSensorHandler::operator=(TempSensorHandler&& rhs) noexcept
{
  device_id_ = std::move(rhs.device_id_);
  sensor_ = std::move(rhs.sensor_);

  setup();

  return *this;
}

mgmt::device::DeviceId_t TempSensorHandler::hardware_id() const
{
  return device_id_;
}

void TempSensorHandler::connect()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::{}", __FUNCTION__);

  sensor_.connect();
}

void TempSensorHandler::async_sync_state()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::{}", __FUNCTION__);

  const auto& temp_sensor = mgmt::device::get_device<mgmt::device::TempSensor_t>(device_id_);
  sensor_.async_set_value(std::to_string(temp_sensor.value()));
}

void TempSensorHandler::setup()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::{}", __FUNCTION__);

  sensor_.set_ack_handler([this]() { set_config(); });
  sensor_.set_error_handler([this](const auto& ec) { on_error(ec); });
}

void TempSensorHandler::set_config()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("TempSensorHandler::{}", __FUNCTION__);

  auto config = mgmt::home_assistant::mqttc::EntityConfig{ sensor_.unique_id() };
  config.set("name", "Temp sensor");
  config.set("device_class", "temperature");
  config.set("unit_of_measurement", "°C");
  config.set("device", mqttc::helper::entity_config_basic_device(identity_provider_.identity(device_id_)));
  sensor_.async_set_config(std::move(config));
  sensor_.async_set_availability(mgmt::home_assistant::mqttc::Availability::Online);

  std::this_thread::sleep_for(std::chrono::seconds(1));// TODO: Workaround
  async_sync_state();
}

void TempSensorHandler::on_error(const mgmt::home_assistant::mqttc::EntityError& error)
{
  spdlog::error("TempSensorHandler::{}, message: {}", __FUNCTION__, error.message());
}
}// namespace mgmt::home_assistant::device