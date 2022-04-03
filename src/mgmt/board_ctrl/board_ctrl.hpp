#pragma once

#include <boost/asio/awaitable.hpp>

#include <mgmt/board_ctrl/board_ctrl_client.hpp>

namespace mgmt::board_ctrl
{
  boost::asio::awaitable<void> async_run(BoardControlClient& client, const std::string& bc_server_address);
}