#pragma once

#include <mgmt/device/device.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::device
{
  class LedDevice : public Device
  {
  public:
    explicit LedDevice(DeviceId_t device_id) : Device(std::move(device_id))
      {}
    virtual boost::asio::awaitable<void> async_set_brightness(uint8_t) const = 0;
  };
}