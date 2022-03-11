#include <hw/platform_device_ctrl/pdctrl_hatch_handler.hpp>

namespace hw::pdctrl {
PlatformDeviceHatchCtrlHandler::DeviceIdCollection_t PlatformDeviceHatchCtrlHandler::available_devices() const
{
  auto device_ids = PlatformDeviceHatchCtrlHandler::DeviceIdCollection_t{};
  const auto& devices = dev_access_.devices();

  std::transform(devices.begin(), devices.end(), std::back_inserter(device_ids), [](const auto& dev) {
    return dev.id();
  });

  return device_ids;
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