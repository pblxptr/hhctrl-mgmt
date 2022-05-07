#pragma once

#include <new/platform_device/loaders/platform_device_creator.hpp>


namespace mgmt::platform_device
{
class PlatformDeviceHatch2SRCreator : public DeviceCreator
{
public:
  bool is_compatible(const std::string&) const override;
  void create(mgmt::device::Device& parent, const PdTreeObject_t&) const override;
};
}