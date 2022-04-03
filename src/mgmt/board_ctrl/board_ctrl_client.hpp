#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/client/basic_client.hpp>
#include <mgmt/board_ctrl/board_info.hpp>

namespace mgmt::board_ctrl
{
  class BoardControlClient : private icon::BasicClient
  {
  public:
  BoardControlClient(boost::asio::io_context&, zmq::context_t&);

  using icon::BasicClient::async_connect;
  using icon::BasicClient::is_connected;

  boost::asio::awaitable<BoardInfo> async_board_info();
  boost::asio::awaitable<void> async_restart();
  };
}