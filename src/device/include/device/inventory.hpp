#pragma once

#include "device_id.hpp"
#include "device.hpp"

namespace mgmt::device {
template<Device D>
class DeviceInventory
{
public:
  template<class... Args>
  constexpr auto emplace(const DeviceId_t& device_id, Args&&... args)
  {
    if (exists(device_id)) {
      throw std::runtime_error("Duplicate instance of the key");
    }

    devices_.emplace(std::piecewise_construct,
      std::tuple(device_id),
      std::forward_as_tuple(std::forward<Args>(args)...));
  }

  constexpr auto add(DeviceId_t device_id, D device)
  {
    if (exists(device_id)) {
      throw std::runtime_error("Duplicate instance of the key");
    }

    devices_.insert({ device_id, std::move(device) });
  }

  constexpr auto get(DeviceId_t device_id) -> D&
  {
    if (not exists(device_id)) {
      throw std::runtime_error("Device does not exist");
    }

    return devices_.at(device_id);
  }

  constexpr auto remove(const DeviceId_t& device_id)
  {
    devices_.erase(device_id);
  }

  constexpr bool exists(const DeviceId_t& device_id) const
  {
    return devices_.contains(device_id);
  }

private:
  std::unordered_map<DeviceId_t, D> devices_;
};
}// namespace mgmt::device
