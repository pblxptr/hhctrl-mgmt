#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/endpoint/endpoint_config.hpp>
#include <icon/endpoint/endpoint.hpp>
#include <icon/endpoint/message_context.hpp>
#include <iconnect/pdci/pdci.pb.h>

#include <hw/platform_device_server/hatch_request_handler.hpp>
#include <hw/platform_device_server/rgbled_request_handler.hpp>

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
        icon::address(address),
        icon::consumer<pdci::GetDeviceIdsReq>([this](auto& context) -> awaitable<void> { co_await handle(context); }),
        icon::consumer<pdci::GetDeviceAttributesReq>([this](auto& context) -> awaitable<void> { co_await handle(context); })
      );

      create_ctrl_adapter<HatchRequestHandler>(builder, devm);
      create_ctrl_adapter<RgbLedRequestHandler>(builder, devm);

      endpoint_ = builder.build();
    }

    boost::asio::awaitable<void> async_run();

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

    boost::asio::awaitable<void> handle(icon::MessageContext<pdci::GetDeviceIdsReq>&) const;
    boost::asio::awaitable<void> handle(icon::MessageContext<pdci::GetDeviceAttributesReq>&) const;
  private:
    std::unique_ptr<icon::Endpoint> endpoint_{};
    std::vector<std::unique_ptr<PlatformDeviceCtrlHandler>> handlers_;
  };
}
