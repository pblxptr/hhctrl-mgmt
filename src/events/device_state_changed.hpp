#pragma once

#include <common/event/base_event.hpp>
#include <device/device_id.hpp>
#include <device/device.hpp>

namespace mgmt::event {
template<mgmt::device::Device D>
struct DeviceStateChanged : public common::event::GenericEvent<DeviceStateChanged<D>>
{
  explicit DeviceStateChanged(mgmt::device::DeviceId_t devid)
    : device_id{ devid }
  {}
  mgmt::device::DeviceId_t device_id;
};
}// namespace mgmt::event