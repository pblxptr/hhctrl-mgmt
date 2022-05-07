#pragma once

#include <icon/endpoint/message_context.hpp>
#include <iconnect/pdci/pdci_hatch.pb.h>
#include <boost/asio/awaitable.hpp>

#include <hw/drivers/led/rgb_led.hpp>
#include <hw/platform_device_server/request_handler.hpp>
#include <hw/platform_device/device_finder.hpp>

namespace hw::pdctrl
{
  class RgbLedRequestHandler : public PlatformDeviceCtrlHandler
  {
    using DeviceFinder_t = hw::platform_device::DeviceFinder<hw::drivers::RGBLedDriver>;
    using PlatformDeviceCtrlHandler::DeviceCollection_t;
  public:
    template<
      class EndpointBuilder,
      class DeviceManager
    >
    RgbLedRequestHandler(
        EndpointBuilder& builder,
        DeviceManager& devm
      )
      : dev_access_{DeviceFinder_t{devm}}
    {}

    DeviceCollection_t available_devices() const override;
  private:
    DeviceFinder_t dev_access_;
  };
}
