#pragma once

#include <functional>

#include <hw/platform_device/device.hpp>

namespace hw::platform_device
{
  template<class DriverInterface>
  class DeviceFinder
  {
  public:
    using DeviceCollection_t = std::vector<GenericDevice<DriverInterface>>;
    using Device_t = GenericDevice<DriverInterface>;

    template<class DeviceManager>
    explicit DeviceFinder(DeviceManager& devm)
      : get_devices_{[&devm]() -> const DeviceCollection_t& { return devm.template devices<DriverInterface>(); }}
      , find_device_{[&devm](const Device::DeviceId_t& devid) { return devm.template device<DriverInterface>(devid); }}
    {}

    decltype(auto) devices() const
    {
      return get_devices_();
    }

    decltype(auto) find_device(const Device::DeviceId_t& devid) const
    {
      return find_device_(devid);
    }

  private:
    std::function<const DeviceCollection_t&()> get_devices_;
    std::function<const Device_t*(const Device::DeviceId_t&)> find_device_;
  };
}
