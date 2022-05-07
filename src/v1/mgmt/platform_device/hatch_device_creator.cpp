#include <mgmt/platform_device/hatch_device_creator.hpp>

namespace mgmt::platform_device
{
bool HatchDeviceCreator::compatible(const DeviceAttributes_t&) const
{
  throw std::runtime_error("Not implemented exception");

  return false;
}

void HatchDeviceCreator::create(mgmt::device::Device&, const DeviceAttributes_t&) const
{
  throw std::runtime_error("Not implemented exception");
}

}