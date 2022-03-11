#include <icon/endpoint/endpoint_config.hpp>

#include <hw/platform_device_ctrl/pdctrl_server.hpp>

namespace hw::pdctrl
{
  PlatformDeviceControlServer::PlatformDeviceControlServer(
    boost::asio::io_context&,
    zmq::context_t&
  )
  {

  }
  boost::asio::awaitable<void> PlatformDeviceControlServer::run(const std::string& endpoint)
  {
    
  }
}