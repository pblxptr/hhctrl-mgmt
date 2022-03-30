#pragma once

#include <mgmt/device/hatch_device.hpp>
#include <mgmt/platform_device/hatch/hatch_pd_client.hpp>

namespace mgmt::platform_device
{
  class HatchPlatformDevice : public mgmt::device::HatchDevice
  {
  public:
    HatchPlatformDevice(mgmt::device::Device::DeviceId_t, std::unique_ptr<mgmt::platform_device::HatchPlatformDeviceClient>);
    boost::asio::awaitable<void> async_open() const override;
    boost::asio::awaitable<void> async_close() const override;
  private:
    std::unique_ptr<mgmt::platform_device::HatchPlatformDeviceClient> client_;
  };
}