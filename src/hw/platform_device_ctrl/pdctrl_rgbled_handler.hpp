#pragma once

#include <icon/endpoint/message_context.hpp>
#include <iconnect/pdci/pdci_hatch.pb.h>
#include <boost/asio/awaitable.hpp>

#include <hw/drivers/led/rgb_led.hpp>
#include <hw/platform_device_ctrl/pdctrl_handler.hpp>
#include <hw/platform_device/device_access.hpp>

namespace hw::pdctrl
{
  class PlatformDeviceRgbLedCtrlHandler : public PlatformDeviceCtrlHandler
  {
    using DeviceAccess_t = hw::platform_device::DeviceAccess<hw::drivers::RGBLedDriver>;
    using PlatformDeviceCtrlHandler::DeviceCollection_t;
  public:
    template<
      class EndpointBuilder,
      class DeviceManager
    >
    PlatformDeviceRgbLedCtrlHandler(
        EndpointBuilder& builder,
        DeviceManager& devm
      )
      : dev_access_{DeviceAccess_t{devm}}
    {}

    DeviceCollection_t available_devices() const override;
  private:
    DeviceAccess_t dev_access_;
  };
}
