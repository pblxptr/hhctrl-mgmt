#pragma once

#include <functional>

#include <hw/platform_device/device.hpp>

namespace hw::platform_device
{
  template<class DeviceInterface>
  class DeviceAccess
  {
  public:
    using DeviceCollection_t = std::vector<GenericDevice<DeviceInterface>>;
    using Device_t = GenericDevice<DeviceInterface>;

    template<class DeviceManager>
    explicit DeviceAccess(DeviceManager& devm)
      : get_devices_{[&devm]() -> const DeviceCollection_t& { return devm.template devices<DeviceInterface>(); }}
      , find_device_{[&devm](const Device::DeviceId_t& devid) { return devm.template device<DeviceInterface>(devid); }}
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
