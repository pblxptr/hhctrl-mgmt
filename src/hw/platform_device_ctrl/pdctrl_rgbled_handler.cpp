#include <hw/platform_device_ctrl/pdctrl_rgbled_handler.hpp>

namespace hw::pdctrl {


PlatformDeviceRgbLedCtrlHandler::DeviceCollection_t PlatformDeviceRgbLedCtrlHandler::available_devices() const
{
  const auto& devices = dev_access_.devices();

  return PlatformDeviceRgbLedCtrlHandler::DeviceCollection_t{devices.begin(), devices.end()};
}
}