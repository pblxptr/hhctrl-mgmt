#include "pdctrl_client.hpp"

#include <spdlog/spdlog.h>
#include <iconnect/pdci/pdci.pb.h>

using boost::asio::awaitable;

namespace {
  constexpr auto RequestTimeout = std::chrono::seconds(0);

  template<class ResponseMessage, class Response>
  void check_if_valid(const Response& response)
  {
    if (response.error_code() || !response.template is<ResponseMessage>()) {
      throw std::runtime_error("Response contains error code or invalid message.");
    }
  }
}

namespace mgmt::pdctrl
{
PlatformDeviceCtrlClient::PlatformDeviceCtrlClient(boost::asio::io_context& bctx, zmq::context_t& zctx)
  : BasicClient(zctx, bctx)
{
  spdlog::get("mgmt")->info("PlatformDeviceCtrlClient: ctor");
}

boost::asio::awaitable<void> PlatformDeviceCtrlClient::devices()
{
  spdlog::get("mgmt")->info("PlatformDeviceCtrlClient: devices");

  auto response = co_await async_send(pdci::GetDeviceIdsReq{}, RequestTimeout);

  check_if_valid<pdci::GetDeviceIdsCfm>(response);

  const auto& message = response.get_safe<pdci::GetDeviceIdsCfm>();

  spdlog::get("mgmt")->info("PlatformDeviceCtrlClient: received devices: {}", message.device_id_size());

  auto device_ids = std::vector<std::string>(message.device_id_size());
  std::copy(message.device_id().begin(), message.device_id().end(), device_ids.begin());

  for (const auto& devid : device_ids) {
    spdlog::get("mgmt")->info("Device id: {}", devid);
    auto req = pdci::GetDeviceAttributesReq{};
    req.set_device_id(devid);

    auto rsp = co_await async_send(std::move(req), RequestTimeout);
    check_if_valid<pdci::GetDeviceAttributesCfm>(rsp);

    const auto& msg = rsp.get_safe<pdci::GetDeviceAttributesCfm>();
    const auto& atr = msg.attribute();
    for (const auto& [k, v] : atr) {
      spdlog::get("mgmt")->info("Attr: '{}' : {}", k, v);
    }
  }
}
}


auto board = BoardDevice{};


event_bus.publish(HatchDeviceCreated{
  device_id, 

});