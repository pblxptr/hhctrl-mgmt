#pragma once

#include <mgmt/device/device.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::device
{
  struct BoardInfo
  {
    std::string model;
    std::string hardware_revision;
    std::string serial_number;
  };
  class BoardDevice : public Device
  {
  public:
    explicit BoardDevice(DeviceId_t device_id) : Device(std::move(device_id))
      {}
    virtual boost::asio::awaitable<void> async_restart() = 0;
    virtual boost::asio::awaitable<BoardInfo> async_board_info() = 0;
  };
}