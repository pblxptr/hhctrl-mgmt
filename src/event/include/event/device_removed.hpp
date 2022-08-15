#pragma once

#include <event/base_event.hpp>
#include <device/device_id.hpp>
#include <device/device.hpp>

namespace mgmt::event {
template<mgmt::device::Device D>
struct DeviceRemoved : public common::event::GenericEvent<DeviceRemoved<D>>
{
  explicit DeviceRemoved(mgmt::device::DeviceId_t devid)
    : device_id{ devid }
  {}
  mgmt::device::DeviceId_t device_id;
};
}// namespace mgmt::event