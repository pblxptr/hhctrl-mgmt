#pragma once

#include <mgmt/device/device.hpp>
#include <mgmt/platform_device/platform_device_client.hpp>

#include <boost/asio/awaitable.hpp>

namespace mgmt::platform_device
{
  // class PlatformDeviceScanner
  // {
  // public:
  //   explicit PlatformDeviceScanner(mgmt::platform_device::PlatformDeviceClient& client)
  //     : client_{client}
  //   {}

  //   virtual boost::asio::awaitable<void> async_scan(mgmt::device::Device& parent)
  //   {
  //     const auto device_ids = co_await client_.async_devices_ids();

  //     // for (const auto& device_id : device_ids) {
  //     //   const attributes = co_await client_.async_device_attributes(device_id);
  //     //   const auto creator = find_creator(attributes);

  //     //   if (creator == nullptr) {
  //     //     throw std::runtime_error("Cannot find device creator");
  //     //   }

  //     //   creator->create(parent, attributes);
  //     // }
  //   }
  // private:
  //   mgmt::platform_device::PlatformDeviceClient& client_;
  // };
}