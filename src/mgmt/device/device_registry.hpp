#pragma once

#include <vector>
#include <algorithm>

#include <mgmt/device/device.hpp>

namespace mgmt::device
{

template<class Device>
class ReadOnlyDeviceRegistry
{
public:
  virtual ~ReadOnlyDeviceRegistry() = default;
  virtual Device* find_device(const DeviceId_t&) const = 0;
  virtual Device& get_device(const DeviceId_t&) const = 0;
};

template<class Device>
class WriteOnlyDeviceRegistry
{
public:
  virtual ~WriteOnlyDeviceRegistry() = default;
  virtual void register_device(Device*) = 0;
  virtual void deregister_device(Device*) = 0;
};

template<class Device>
class DeviceRegistry : public WriteOnlyDeviceRegistry<Device>, public ReadOnlyDeviceRegistry<Device>
{
public:
  void register_device(Device* device) override
  {
    assert(device != nullptr);

    auto dev = std::find_if(devices_.begin(), devices_.end(), [&device](auto& entry_dev){
      return entry_dev->id() == device->id();
    });

    if (dev == devices_.end()) {
      devices_.push_back(device);
    } else {
      throw std::runtime_error("Device already exist");
    }
  }
  void deregister_device(Device* device) override
  {
    assert(device != nullptr);

    std::erase_if(devices_, [&device](const auto& entry_dev) {
      return entry_dev->id() == device->id();
    });
  }

  Device* find_device(const DeviceId_t& device_id) const override
  {
    auto dev = std::find_if(devices_.begin(), devices_.end(), [&device_id](auto& entry_dev){
      return entry_dev->id() == device_id;
    });

    if (dev == devices_.end()) {
      throw std::runtime_error("Cannot find device");
    }

    return *dev;
  }

  Device& get_device(const DeviceId_t& device_id) const override
  {
    auto dev = std::find_if(devices_.begin(), devices_.end(), [&device_id](auto& entry_dev){
      return entry_dev->id() == device_id;
    });

    if (dev == devices_.end()) {
      throw std::runtime_error("Cannot find device");
    }

    return *(*dev);
  }
private:
  std::vector<Device*> devices_;
};
}