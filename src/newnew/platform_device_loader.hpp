#pragma once

#include <newnew/device.hpp>
#include <newnew/pdtree.hpp>

namespace mgmt::platform_device
{
class DeviceLoader
{
public:
  ~DeviceLoader() = default;
  virtual bool is_compatible(const std::string&) const = 0;
  virtual void load(mgmt::device::Device&, const PdTreeObject_t&) const = 0;
};
}