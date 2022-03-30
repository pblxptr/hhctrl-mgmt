#include <hw/platform_device_ctrl/pdctrl_hatch_handler.hpp>

namespace hw::pdctrl {
PlatformDeviceHatchCtrlHandler::DeviceCollection_t PlatformDeviceHatchCtrlHandler::available_devices() const
{
  const auto& devices = dev_access_.devices();

  return PlatformDeviceHatchCtrlHandler::DeviceCollection_t(devices.begin(), devices.end());
}

boost::asio::awaitable<void> PlatformDeviceHatchCtrlHandler::handle(icon::MessageContext<pdci::hatch::GetStatusReq>&)
{
  co_return;
}

boost::asio::awaitable<void> PlatformDeviceHatchCtrlHandler::handle(icon::MessageContext<pdci::hatch::OpenHatchReq>&)
{
  co_return;
}

boost::asio::awaitable<void> PlatformDeviceHatchCtrlHandler::handle(icon::MessageContext<pdci::hatch::CloseHatchReq>&)
{
  co_return;
}
}