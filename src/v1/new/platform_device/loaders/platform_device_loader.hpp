#pragma once

#include <mgmt/device/device.hpp>
#include <new/platform_device/pdtree/pdtree.hpp>

namespace mgmt::platform_device
{
class PlatfomDeviceLoader
{
public:
  ~PlatfomDeviceLoader() = default;
  virtual bool is_compatible(const std::string&) const = 0;
  virtual void load(mgmt::device::Device&, const PdTreeObject_t&) const = 0;
};
}