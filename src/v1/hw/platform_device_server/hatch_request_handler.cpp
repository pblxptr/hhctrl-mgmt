#include <hw/platform_device_server/hatch_request_handler.hpp>

namespace hw::pdctrl {
HatchRequestHandler::DeviceCollection_t HatchRequestHandler::available_devices() const
{
  const auto& devices = dev_access_.devices();

  return HatchRequestHandler::DeviceCollection_t(devices.begin(), devices.end());
}

boost::asio::awaitable<void> HatchRequestHandler::handle(icon::MessageContext<pdci::hatch::GetStatusReq>&)
{
  co_return;
}

boost::asio::awaitable<void> HatchRequestHandler::handle(icon::MessageContext<pdci::hatch::OpenHatchReq>&)
{
  co_return;
}

boost::asio::awaitable<void> HatchRequestHandler::handle(icon::MessageContext<pdci::hatch::CloseHatchReq>&)
{
  co_return;
}
}