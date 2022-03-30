#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/client/basic_client.hpp>

namespace mgmt::pdctrl
{
  class PlatformDeviceCtrlClient : private icon::BasicClient
  {
  public:
    PlatformDeviceCtrlClient(boost::asio::io_context&, zmq::context_t&);

    boost::asio::awaitable<void> devices();

    using icon::BasicClient::async_connect;
    using icon::BasicClient::is_connected;
  };
}