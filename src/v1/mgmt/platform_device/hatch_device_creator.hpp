#pragma once

#include <mgmt/platform_device/device_creator.hpp>

namespace mgmt::platform_device
{
  class HatchDeviceCreator : public DeviceCreator
  {
  public:
    virtual bool compatible(const DeviceAttributes_t&) const override;
    virtual void create(mgmt::device::Device&, const DeviceAttributes_t&) const override;
  };
}