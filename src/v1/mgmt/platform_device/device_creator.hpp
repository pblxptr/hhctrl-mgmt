#pragma once

#include <mgmt/device/device.hpp>
#include <mgmt/platform_device/attributes.hpp>

namespace mgmt::platform_device
{
  class DeviceCreator
  {
  public:
    virtual ~DeviceCreator() = default;

    virtual bool compatible(const DeviceAttributes_t&) const = 0;
    virtual void create(mgmt::device::Device&, const DeviceAttributes_t&) const = 0;
  };
}