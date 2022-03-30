#pragma once

#include <mgmt/device/device.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::device
{
  class HatchDevice : public Device
  {
  public:
    explicit HatchDevice(DeviceId_t device_id) : Device(std::move(device_id))
      {}
    virtual boost::asio::awaitable<void> async_open() const = 0;
    virtual boost::asio::awaitable<void> async_close() const = 0;
  };
}