#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/endpoint/endpoint_config.hpp>
#include <icon/endpoint/endpoint.hpp>
#include <icon/endpoint/message_context.hpp>
#include <hw/services/led_service.hpp>
#include <hw/platform_device_ctrl/pdctrl_handler.hpp>
#include <hw/platform_device_ctrl/pdctrl_hatch_handler.hpp>
#include <common/traits/tuple_traits.hpp>

#include <string>

namespace hw::pdctrl
{
  class PlatformDeviceCtrlServer
  {
  public:
    template<class DeviceManager>
    PlatformDeviceCtrlServer(
      boost::asio::io_context& bctx,
      zmq::context_t& zctx,
      DeviceManager& devm,
      const std::string& address
    )
    {
      auto builder = icon::setup_default_endpoint(
        icon::use_services(bctx, zctx),
        icon::address(address)
      );

      create_ctrl_adapter<PlatformDeviceHatchCtrlHandler>(builder, devm);

      endpoint_ = builder.build();
    }

    boost::asio::awaitable<void> run()
    {
      co_return;
    }

  private:
    template<
      class Handler,
      class Builder,
      class DeviceManager
    >
    void create_ctrl_adapter(Builder& builder, DeviceManager& devm)
    {
      handlers_.push_back(std::make_unique<Handler>(builder, devm));
    }

  private:
    std::unique_ptr<icon::Endpoint> endpoint_{};
    std::vector<std::unique_ptr<PlatformDeviceCtrlHandler>> handlers_;
  };
}
