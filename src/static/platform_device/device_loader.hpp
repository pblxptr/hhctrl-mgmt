#pragma once

#include <static/device/device_id.hpp>

namespace mgmt::platform_device
{
  struct DeviceLoader
  {
    std::function<mgmt::device::DeviceId_t()> load;
    std::function<void(mgmt::device::DeviceId_t)> unload;
  };
}