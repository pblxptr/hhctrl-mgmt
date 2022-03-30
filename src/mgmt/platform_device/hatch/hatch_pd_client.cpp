#include <mgmt/platform_device/hatch/hatch_pd_client.hpp>

namespace {
  template<class T>
  using Awaitable_t = boost::asio::awaitable<T>;

  constexpr auto RequestTimeout = std::chrono::seconds(10);
}

namespace mgmt::platform_device
{
  HatchPlatformDeviceClient::HatchPlatformDeviceClient(std::unique_ptr<icon::BasicClient> client, std::string server_address)
    : client_{std::move(client)}
    , server_address_{std::move(server_address)}
  {
    spdlog::get("mgmt")->debug("HatchPlatformDeviceClient: ctor");
  }

Awaitable_t<pdci::hatch::GetStatusCfm> HatchPlatformDeviceClient::async_send(pdci::hatch::GetStatusReq req) const
{
  spdlog::get("mgmt")->debug("HatchPlatformDeviceClient: async_send(pdci::hatch::GetStatusReq)");

  co_await async_connect_if_needed();

  auto response = co_await client_->async_send(std::move(req), RequestTimeout);

  co_return response.get_safe<pdci::hatch::GetStatusCfm>();
}

Awaitable_t<pdci::hatch::OpenHatchCfm> HatchPlatformDeviceClient::async_send(pdci::hatch::OpenHatchReq req) const
{
  spdlog::get("mgmt")->debug("HatchPlatformDeviceClient: async_send(pdci::hatch::OpenHatchReq)");

  co_await async_connect_if_needed();

  auto response = co_await client_->async_send(std::move(req), RequestTimeout);

  co_return response.get_safe<pdci::hatch::OpenHatchCfm>();
}

Awaitable_t<pdci::hatch::CloseHatchCfm> HatchPlatformDeviceClient::async_send(pdci::hatch::CloseHatchReq req) const
{
  spdlog::get("mgmt")->debug("HatchPlatformDeviceClient: async_send(pdci::hatch::CloseHatchReq)");

  co_await async_connect_if_needed();

  auto response = co_await client_->async_send(std::move(req), RequestTimeout);

  co_return response.get_safe<pdci::hatch::CloseHatchCfm>();
}

Awaitable_t<void> HatchPlatformDeviceClient::async_connect_if_needed() const
{
  spdlog::get("mgmt")->debug("HatchPlatformDeviceClient: async_connect_if_needed");

  if (client_->is_connected()) {
    co_return;
  }

  co_await client_->async_connect(server_address_.c_str());
}
}

