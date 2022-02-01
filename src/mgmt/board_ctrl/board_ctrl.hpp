#pragma once

#include <boost/asio/awaitable.hpp>

#include <mgmt/board_ctrl/board_ctrl_client.hpp>

namespace mgmt::board_ctrl
{
  class BoardController
  {
  public:
    explicit BoardController(mgmt::board_ctrl::BoardControlClient&);
    boost::asio::awaitable<void> connect(const std::string&);

  private:
    boost::asio::awaitable<void> setup_default();
  private:
    mgmt::board_ctrl::BoardControlClient& client_;
  };
}