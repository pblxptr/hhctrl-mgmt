#include <mgmt/platform_device/platform_device_client.hpp>

#include <spdlog/spdlog.h>
#include <iconnect/pdci/pdci.pb.h>

using boost::asio::awaitable;

namespace mgmt::platform_device
{
PlatformDeviceClient::PlatformDeviceClient(common::utils::Client client)
  : client_{std::move(client)}
{
  spdlog::get("mgmt")->info("PlatformDeviceClient: ctor");
}

  boost::asio::awaitable<std::vector<DeviceId_t>> PlatformDeviceClient::async_devices_ids()
  {
    spdlog::get("mgmt")->info("PlatformDeviceClient: async_devices_ids");

    auto response = co_await client_.async_send(pdci::GetDeviceIdsReq{});

    const auto& message = response.get_safe<pdci::GetDeviceIdsCfm>();

    spdlog::get("mgmt")->info("PlatformDeviceClient: received devices: {}", message.device_id_size());

    auto device_ids = std::vector<std::string>(message.device_id_size());
    std::copy(message.device_id().begin(), message.device_id().end(), device_ids.begin());

    co_return device_ids;
  }

  boost::asio::awaitable<DeviceAttributes_t> PlatformDeviceClient::async_device_attributes(const DeviceId_t& device_id)
  {
    spdlog::get("mgmt")->info("PlatformDeviceClient: async_device_attributes");

    auto req = pdci::GetDeviceAttributesReq{};
    req.set_device_id(device_id);

    auto response = co_await client_.async_send(std::move(req));
    const auto& msg = response.get_safe<pdci::GetDeviceAttributesCfm>();
    const auto& attributes = msg.attributes();

    co_return DeviceAttributes_t{attributes.begin(), attributes.end()};
  }
}

