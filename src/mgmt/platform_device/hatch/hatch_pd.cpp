#include <mgmt/platform_device/hatch/hatch_pd.hpp>

namespace {
  template<class T>
  using Awaitable_t = boost::asio::awaitable<T>;
}

namespace mgmt::platform_device
{
  HatchPlatformDevice::HatchPlatformDevice(mgmt::device::Device::DeviceId_t device_id, std::unique_ptr<HatchPlatformDeviceClient> client)
    : HatchDevice(std::move(device_id))
    , client_(std::move(client))
  {}

  Awaitable_t<void> HatchPlatformDevice::async_open() const
  {
    auto req = pdci::hatch::OpenHatchReq{};
    req.set_device_id(HatchDevice::id());

    co_await client_->async_send(std::move(req));
  }

  Awaitable_t<void> HatchPlatformDevice::async_close() const
  {
    auto req = pdci::hatch::CloseHatchReq{};
    req.set_device_id(HatchDevice::id());

    co_await client_->async_send(std::move(req));
  }
}