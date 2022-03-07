#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/endpoint/endpoint.hpp>
#include <icon/endpoint/message_context.hpp>
#include <hw/services/led_service.hpp>

#include <string>

namespace hw::pdctrl
{
  class PlatformDeviceControlServer
  {
  public:
    PlatformDeviceServer(
      boost::asio::io_context&,
      zmq::context_t&
    );

    template<class T>
    void create_driver_ctrl_adapter()

    boost::asio::awaitable<void> run(const std::string& endpoint);

  private:
    std::unique_ptr<icon::Endpoint> endpoint_{};
  };
}
