#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include <icon/client/basic_client.hpp>
#include <unordered_map>

#include <common/utils/client.hpp>
#include <mgmt/platform_device/device_id.hpp>
#include <mgmt/platform_device/attributes.hpp>
#include <mgmt/device/device.hpp>

namespace mgmt::platform_device
{
  class PlatformDeviceClient
  {
  public:
    explicit PlatformDeviceClient(common::utils::Client);

    boost::asio::awaitable<std::vector<DeviceId_t>> async_devices_ids();
    boost::asio::awaitable<DeviceAttributes_t> async_device_attributes(const DeviceId_t&);
  private:
    common::utils::Client client_;
  };
}