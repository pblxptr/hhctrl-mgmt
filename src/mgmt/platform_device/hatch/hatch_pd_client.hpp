#pragma once

#include <icon/client/basic_client.hpp>
#include <iconnect/pdci/pdci_hatch.pb.h>

namespace mgmt::platform_device
{
  class HatchPlatformDeviceClient
  {
  public:
    virtual ~HatchPlatformDeviceClient() = default;

    HatchPlatformDeviceClient(std::unique_ptr<icon::BasicClient>, std::string server_address);

    virtual boost::asio::awaitable<pdci::hatch::GetStatusCfm> async_send(pdci::hatch::GetStatusReq) const;
    virtual boost::asio::awaitable<pdci::hatch::OpenHatchCfm> async_send(pdci::hatch::OpenHatchReq) const;
    virtual boost::asio::awaitable<pdci::hatch::CloseHatchCfm> async_send(pdci::hatch::CloseHatchReq) const;

  private:
    boost::asio::awaitable<void> async_connect_if_needed() const;

  private:
    std::unique_ptr<icon::BasicClient> client_;
    const std::string server_address_;
  };
}
