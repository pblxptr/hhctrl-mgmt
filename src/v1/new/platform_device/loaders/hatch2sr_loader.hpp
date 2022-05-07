#pragma once

#include <new/platform_device/loaders/platform_device_loader.hpp>

namespace mgmt::platform_device
{
class Hatch2SRLoader : public PlatfomDeviceLoader
{
public:
  bool is_compatible(const std::string&) const override;
  void load(mgmt::device::Device& parent, const PdTreeObject_t&) const override;
};
}