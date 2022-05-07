#pragma once

#include <mgmt/device/device.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::device
{
  enum class HatchStatus { Undefined, Open, Closed, ChangingPosition, Faulty };

  class HatchDevice : public Device
  {
  public:
    explicit HatchDevice(DeviceId_t device_id) : Device(std::move(device_id))
      {}
    virtual boost::asio::awaitable<HatchStatus> async_state() = 0;
    virtual boost::asio::awaitable<void> async_open() = 0;
    virtual boost::asio::awaitable<void> async_close() = 0;
  };
}