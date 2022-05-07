#pragma once

#include <vector>
#include <utility>
#include <stdexcept>
#include <ranges>
#include <spdlog/spdlog.h>

#include <newnew/device.hpp>

/**
  TODO: Consider changing Value_t from reference_wrapper<Device> to something stronger
  that would ensure that once the device is about to destroy, reference will be removed from store.
  At this moment creating disposable_device with custom on_disponse function is enought but something
  like StorableDevice (that would have appropriate deleter by default) would be better.
  make_storable<HatchDevice>(store)
    .on_dispose())
    .on_dispose()
    .on_dispose()
    .make("params");
 */



namespace mgmt::device
{
class DeviceRefRegistry
{
  using Value_t = std::reference_wrapper<Device>;
public:
  void add(Value_t device)
  {
    using std::to_string;

    spdlog::get("mgmt")->debug("Adding device with id: {}", to_string(device.get().id()));

    if (std::ranges::any_of(devices_, [&device](const auto& dev) {
      return device.get().id() == dev.get().id();
    })) {
      throw std::runtime_error("Device already exists");
    }

    devices_.push_back(std::move(device));
  }

  bool exists(const DeviceId_t& device_id) const
  {
    return std::ranges::find_if(devices_, [&device_id](const auto& dev) {
      return dev.get().id() == device_id;
    }) != devices_.end();
  }

  template<class T>
  T& get(const DeviceId_t& device_id) const
  {
    using std::to_string;

    spdlog::get("mgmt")->debug("Getting device with id: {}", to_string(device_id));

    auto device = std::ranges::find_if(devices_, [&device_id](const auto& dev) {
      return dev.get().id() == device_id;
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
    using std::to_string;

    spdlog::get("mgmt")->debug("Removing device with id: {}", to_string(device_id));

    auto dev = std::ranges::find_if(devices_, [&device_id](const auto& dev) {
      return dev.get().id() == device_id;
    });

    if (dev != devices_.end()) {
      devices_.erase(dev);
    }
  }

private:
  std::vector<Value_t> devices_;
};
}
