#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/client/basic_client.hpp>
#include <common/data/indicator.hpp>

namespace mgmt::board_ctrl
{
  class BoardControlClient : private icon::BasicClient
  {
  public:
  BoardControlClient(boost::asio::io_context&, zmq::context_t&);

  using icon::BasicClient::async_connect;
  using icon::BasicClient::is_connected;

  boost::asio::awaitable<void> set_visual_indication(const common::data::IndicatorType&,
      const common::data::IndicatorState&);
  boost::asio::awaitable<common::data::IndicatorState> get_visual_indication(
    const common::data::IndicatorType&);
  };
}