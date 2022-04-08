#pragma once

#include <mgmt/device/board_device.hpp>
#include <common/utils/client.hpp>

namespace mgmt::board_ctrl
{
  class MainBoardDevice : public mgmt::device::BoardDevice
  {
  public:
    explicit MainBoardDevice(common::utils::Client);
    boost::asio::awaitable<void> async_restart() override;
    boost::asio::awaitable<mgmt::device::BoardInfo> async_board_info() override;
  private:
    common::utils::Client client_;
  };
}