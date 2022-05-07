#pragma once

#include <mgmt/device/device.hpp>
#include <new/platform_device/loaders/platform_device_loader.hpp>

namespace mgmt::platform_device
{
  class DeviceScanner
  {
    using DeviceLoader_t = std::unique_ptr<PlatfomDeviceLoader>;
  public:
    ~DeviceScanner() = default;
    explicit DeviceScanner(const std::string&, std::vector<DeviceLoader_t>);
    virtual void scan(mgmt::device::Device&);
  private:
    std::string path_;
    std::vector<DeviceLoader_t> loaders_;
  };
}