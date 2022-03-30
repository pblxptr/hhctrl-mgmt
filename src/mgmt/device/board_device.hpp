#pragma once

namespace mgmt::device
{
#pragma once

#include <mgmt/device/device.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::device
{
  class BoardDevice : public Device
  {
  public:
    explicit BoardDevice(DeviceId_t device_id) : Device(std::move(device_id))
      {}
    virtual boost::asio::awaitable<void> async_restart() const = 0;
  };
}
}