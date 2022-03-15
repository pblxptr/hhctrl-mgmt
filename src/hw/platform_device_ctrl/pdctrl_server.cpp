#include <icon/endpoint/endpoint_config.hpp>
#include <spdlog/spdlog.h>

#include <hw/platform_device_ctrl/pdctrl_server.hpp>

namespace hw::pdctrl
{
  boost::asio::awaitable<void> PlatformDeviceCtrlServer::run()
  {
    spdlog::get("hw")->info("PlatformDeviceCtrlServer: run");

    co_await endpoint_->run();

    spdlog::get("hw")->info("PlatformDeviceCtrlServer: run finished");
  }

  boost::asio::awaitable<void> PlatformDeviceCtrlServer::handle(icon::MessageContext<pdci::GetDeviceIdsReq>& context)
  {
    auto response = pdci::GetDeviceIdsCfm{};

    for (const auto& handler : handlers_) {
      for (const auto& device_ids : handler->available_devices()) {
        for (const auto& dev_id : device_ids) {
          response.add_device_id(device_ids);
        }
      }
    }

    co_await context.async_respond(std::move(response));
  }
}