#pragma once

#include <memory>
#include <algorithm>
#include <ranges>
#include <vector>
#include <fmt/format.h>

#include <newnew/device_id.hpp>
#include <newnew/device.hpp>

namespace mgmt::device
{
struct DeviceRegistry
{
public:
  void add(std::unique_ptr<Device> device)
  {
    if (std::ranges::any_of(devices_, [&device](const auto& dev) {
      return device->id() == dev->id();
    })) {
      throw std::runtime_error("Device already exists");
    }

    devices_.push_back(std::move(device));
  }
  template<class T>
  T& get(const DeviceId_t& device_id) const
  {
    auto device = std::ranges::find_if(devices_, [&device_id](const auto& d) {
      return d->id() == device_id;
    });
    if (device == devices_.end()) {
      throw std::runtime_error("Cannot find device");
    }

    auto device_type = mgmt::device::runtime_device_type<T>();

    if (device_type != (*device)->type()) {
      throw std::runtime_error("Types mismatch");
    }

    return static_cast<T&>(*(*device));
  }

  void remove(const DeviceId_t& device_id)
  {
    auto dev = std::ranges::find_if(devices_, [&device_id](const auto& dev) {
      return dev->id() == device_id;
    });

    if (dev != devices_.end()) {
      devices_.erase(dev);
    }
  }

private:
  std::vector<std::unique_ptr<Device>> devices_;
};
}