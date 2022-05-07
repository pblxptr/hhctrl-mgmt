#include <mgmt/platform_device/platform_device_hatch.hpp>
#include <iconnect/pdci/pdci_hatch.pb.h>
namespace {
  template<class T>
  using Awaitable_t = boost::asio::awaitable<T>;
}

namespace mgmt::platform_device
{
  PlatformDeviceHatch::PlatformDeviceHatch(mgmt::device::DeviceId_t device_id, common::utils::Client client)
    : HatchDevice(std::move(device_id))
    , client_(std::move(client))
  {}

  Awaitable_t<mgmt::device::HatchStatus> PlatformDeviceHatch::async_state()
  {
    auto req = pdci::hatch::GetStatusReq{};
    req.set_device_id(HatchDevice::id().value());

    const auto response = co_await client_.async_send(std::move(req));
    const auto& message = response.get_safe<pdci::hatch::GetStatusCfm>();

    switch (message.status()) {
      case pdci::hatch::HatchStatus::Undefined:
        co_return mgmt::device::HatchStatus::Undefined;

      case pdci::hatch::HatchStatus::Open:
        co_return mgmt::device::HatchStatus::Open;

      case pdci::hatch::HatchStatus::Closed:
        co_return mgmt::device::HatchStatus::Closed;

      case pdci::hatch::HatchStatus::ChangingPosition:
        co_return mgmt::device::HatchStatus::ChangingPosition;

      case pdci::hatch::HatchStatus::Faulty:
        co_return mgmt::device::HatchStatus::Faulty;
    }
  }

  Awaitable_t<void> PlatformDeviceHatch::async_open()
  {
    auto req = pdci::hatch::OpenHatchReq{};
    req.set_device_id(HatchDevice::id().value());

    co_await client_.async_send(std::move(req));
  }

  Awaitable_t<void> PlatformDeviceHatch::async_close()
  {
    auto req = pdci::hatch::CloseHatchReq{};
    req.set_device_id(HatchDevice::id().value());

    co_await client_.async_send(std::move(req));
  }
}