#pragma once

#include <mgmt/device/hatch_device.hpp>
#include <common/utils/client.hpp>

namespace mgmt::platform_device
{
  class PlatformDeviceHatch : public mgmt::device::HatchDevice
  {
  public:
    PlatformDeviceHatch(mgmt::device::DeviceId_t, common::utils::Client);
    boost::asio::awaitable<mgmt::device::HatchStatus> async_state() override;
    boost::asio::awaitable<void> async_open() override;
    boost::asio::awaitable<void> async_close() override;
  private:
    common::utils::Client client_;
  };
}