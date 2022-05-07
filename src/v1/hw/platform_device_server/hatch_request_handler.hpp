#pragma once

#include <icon/endpoint/message_context.hpp>
#include <iconnect/pdci/pdci_hatch.pb.h>
#include <boost/asio/awaitable.hpp>

#include <hw/drivers/misc/hatch.hpp>
#include <hw/platform_device_server/request_handler.hpp>
#include <hw/platform_device/device_finder.hpp>

namespace hw::pdctrl
{
  class HatchRequestHandler : public PlatformDeviceCtrlHandler
  {
    using DeviceFinder_t = hw::platform_device::DeviceFinder<hw::drivers::HatchDriver>;
    using PlatformDeviceCtrlHandler::DeviceCollection_t;
  public:
    template<
      class EndpointBuilder,
      class DeviceManager
    >
    HatchRequestHandler(
        EndpointBuilder& builder,
        DeviceManager& devm
      )
      : dev_access_{DeviceFinder_t{devm}}
    {
      builder.template add_consumer<pdci::hatch::GetStatusReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); });

      builder.template add_consumer<pdci::hatch::OpenHatchReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); });

      builder.template add_consumer<pdci::hatch::CloseHatchReq>([this](auto& context)
        -> awaitable<void> { co_await handle(context); });
    }

    DeviceCollection_t available_devices() const override;

  private:
    boost::asio::awaitable<void> handle(icon::MessageContext<pdci::hatch::GetStatusReq>&);
    boost::asio::awaitable<void> handle(icon::MessageContext<pdci::hatch::OpenHatchReq>&);
    boost::asio::awaitable<void> handle(icon::MessageContext<pdci::hatch::CloseHatchReq>&);
  private:
    DeviceFinder_t dev_access_;
  };
}
