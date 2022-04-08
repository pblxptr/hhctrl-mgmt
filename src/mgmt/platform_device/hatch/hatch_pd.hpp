#pragma once

#include <mgmt/device/hatch_device.hpp>
#include <common/utils/client.hpp>

namespace mgmt::platform_device
{
  class HatchPlatformDevice : public mgmt::device::HatchDevice
  {
  public:
    HatchPlatformDevice(mgmt::device::DeviceId_t, common::utils::Client);
    boost::asio::awaitable<void> async_open() override;
    boost::asio::awaitable<void> async_close() override;
  private:
    common::utils::Client client_;
  };
}