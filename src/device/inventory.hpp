#pragma once

#include <device/device_id.hpp>
#include <device/device.hpp>

namespace mgmt::device {
template<Device D>
class Inventory
{
public:
  template<class... Args>
  constexpr auto emplace(const DeviceId_t& id, Args&&... args)
  {
    if (exists(id)) {
      throw std::runtime_error("Duplicate instance of the key");
    }

    devices_.emplace(std::piecewise_construct,
      std::tuple(id),
      std::forward_as_tuple(std::forward<Args>(args)...));
  }

  constexpr auto add(DeviceId_t id, D device)
  {
    if (exists(id)) {
      throw std::runtime_error("Duplicate instance of the key");
    }

    devices_.insert({ id, std::move(device) });
  }

  constexpr auto get(DeviceId_t id) -> D&
  {
    if (not exists(id)) {
      throw std::runtime_error("Device does not exist");
    }

    return devices_.at(id);
  }

  constexpr auto remove(const DeviceId_t& id)
  {
    devices_.erase(id);
  }

  constexpr bool exists(const DeviceId_t& id) const
  {
    return devices_.contains(id);
  }

private:
  std::unordered_map<DeviceId_t, D> devices_;
};
}// namespace mgmt::device