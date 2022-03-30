#pragma once

#include <hw/platform_device/device.hpp>

namespace hw::platform_device
{
  template<class DriverInterface>
  class DeviceResource
  {
    using Device_t = GenericDevice<DriverInterface>;
    using DriverPtr_t = std::unique_ptr<DriverInterface>;
  public:
    const Device_t* device(const std::string& device_id) const
    {
      auto device_it = std::find_if(devices_.begin(), devices_.end(), [&device_id](auto& dev)
      {
        return dev.id() == device_id;
      });

      if (device_it == devices_.end()) {
        return nullptr;
      }

      return &*device_it;
    }

    const std::vector<Device_t>& devices() const
    {
      return devices_;
    }

    DriverInterface* register_dev_resource(Device::DeviceId_t dev_id, DriverPtr_t driver, DeviceAttributes attrs)
    {
      devices_.emplace_back(std::move(dev_id), std::move(attrs), std::move(driver));

      return devices_.back().driver();
    }
  private:
    std::vector<Device_t> devices_;;
};
}