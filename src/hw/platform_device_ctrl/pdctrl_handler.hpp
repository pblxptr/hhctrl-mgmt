#pragma once

#include <vector>

#include <hw/platform_device/device.hpp>

namespace hw::pdctrl
{
  class PlatformDeviceCtrlHandler
  {
  public:
    using DeviceIdCollection_t = std::vector<hw::platform_device::DeviceBase::DeviceId_t>;

    virtual ~PlatformDeviceCtrlHandler() = default;
    virtual DeviceIdCollection_t available_devices() const = 0;
  };
}