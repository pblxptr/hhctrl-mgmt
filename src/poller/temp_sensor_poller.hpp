//
// Created by pp on 7/27/22.
//

#pragma once

#include <boost/functional/hash.hpp>
#include <boost/asio/awaitable.hpp>

#include <device/temp_sensor_t.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>
#include <common/event/event_bus.hpp>
#include <events/device_state_changed.hpp>
#include <poller/logger.hpp>

namespace mgmt::poller {
class TempSensorPoller
{
public:
  TempSensorPoller(mgmt::device::DeviceId_t device_id, common::event::AsyncEventBus& bus)
    : device_id_{ std::move(device_id) }
    , bus_{ bus }
    , cached_temp_{}
  {
  }

  boost::asio::awaitable<void> operator()()
  {
    const auto tmp_val = current_temp();
    if (tmp_val != cached_temp_) {
      update(tmp_val);
    }
    common::logger::get(mgmt::poller::Logger)->debug("Polling TempSensorP({}): {}", device_id_, tmp_val == cached_temp_ ? "up to date" : "changed");
    co_return;
  }

private:
  float current_temp() const
  {
    const auto& temp_sensor = mgmt::device::get_device<mgmt::device::TempSensor_t>(device_id_);

    return temp_sensor.value();
  }

  void update(float new_value)
  {
    cached_temp_ = new_value;
    bus_.publish(mgmt::event::DeviceStateChanged<mgmt::device::TempSensor_t>(device_id_));
  }

private:
  mgmt::device::DeviceId_t device_id_;
  common::event::AsyncEventBus& bus_;
  float cached_temp_;
};

}// namespace mgmt::poller
