#pragma once

#include <vector>

#include <hw/platform_device/device.hpp>

namespace hw::pdctrl
{
  class PlatformDeviceCtrlHandler
  {
  public:
    using DeviceCollection_t = std::vector<std::reference_wrapper<const hw::platform_device::Device>>;

    virtual ~PlatformDeviceCtrlHandler() = default;
    virtual DeviceCollection_t available_devices() const = 0;
  };
}
