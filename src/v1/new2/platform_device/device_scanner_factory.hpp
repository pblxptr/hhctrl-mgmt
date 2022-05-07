#pragma once

#include <new/platform_device/services/device_scaner.hpp>
#include <new/platform_device/loaders/hatch2sr_loader.hpp>
#include <new/platform_device/loaders/sysfs_led_loader.hpp>
#include <new/platform_device/loaders/sysfs_rgbled_loader.hpp>

namespace mgmt::platform_device
{
  class DeviceScannerFactory
  {
  public:
    DeviceScannerFactory(std::string pdtree_file)
      : pdtree_file_(std::move(pdtree_file))
    {}

    std::unique_ptr<DeviceScanner> create()
    {
      auto loaders = std::vector<std::unique_ptr<PlatfomDeviceLoader>>{};
      loaders.push_back(std::make_unique<mgmt::platform_device::Hatch2SRLoader>());
      loaders.push_back(std::make_unique<mgmt::platform_device::SysfsLedLoader>());
      loaders.push_back(std::make_unique<mgmt::platform_device::SysfsRGBLedLoader>());

      return mgmt::platform_device::DeviceScanner{pdtree_file_, std::move(loaders)};
    }
  private:
    std::string pdtree_file_;
  };
}