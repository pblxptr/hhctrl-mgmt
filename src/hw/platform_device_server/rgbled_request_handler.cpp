#include <hw/platform_device_server/rgbled_request_handler.hpp>

namespace hw::pdctrl {


RgbLedRequestHandler::DeviceCollection_t RgbLedRequestHandler::available_devices() const
{
  const auto& devices = dev_access_.devices();

  return RgbLedRequestHandler::DeviceCollection_t{devices.begin(), devices.end()};
}
}