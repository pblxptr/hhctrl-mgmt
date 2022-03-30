#include <icon/endpoint/endpoint_config.hpp>
#include <spdlog/spdlog.h>

#include <hw/platform_device_server/pdctrl_server.hpp>

namespace {
  template<class Collection, class DeviceId>
  std::optional<std::reference_wrapper<const hw::platform_device::Device>> find_device(const Collection& handlers, const DeviceId& dev_id)
  {
    for (const auto& handler : handlers) {
      const auto& devices = handler->available_devices();

      auto dev = std::find_if(devices.begin(), devices.end(), [&dev_id](const auto& device){
        return device.get().id() == dev_id;
      });

      if (dev != devices.end()) {
        return std::optional(*dev);
      }
    }

    return std::nullopt;
  }
}

namespace hw::pdctrl
{
  boost::asio::awaitable<void> PlatformDeviceCtrlServer::run()
  {
    spdlog::get("hw")->info("PlatformDeviceCtrlServer: run");

    co_await endpoint_->run();

    spdlog::get("hw")->info("PlatformDeviceCtrlServer: run finished");
  }

  boost::asio::awaitable<void> PlatformDeviceCtrlServer::handle(icon::MessageContext<pdci::GetDeviceIdsReq>& context) const
  {
    auto response = pdci::GetDeviceIdsCfm{};

    for (const auto& handler : handlers_) {
      for (const auto& device : handler->available_devices()) {
        response.add_device_id(device.get().id());
      }
    }

    co_await context.async_respond(std::move(response));
  }

  boost::asio::awaitable<void> PlatformDeviceCtrlServer::handle(icon::MessageContext<pdci::GetDeviceAttributesReq>& context) const
  {
    auto response = pdci::GetDeviceAttributesCfm{};
    const auto& device_id = context.message().device_id();

    auto device = find_device(handlers_, device_id);
    if (not device) {
      throw std::runtime_error("Cannot find device with requested id");
    }

    using std::to_string;
    auto& attributes_map = *response.mutable_attribute();

    for (auto&& [key, val] : device->get().attributes()) {
      attributes_map[std::forward<decltype(key)>(key)] = to_string(std::forward<decltype(val)>(val));
    }

    co_await context.async_respond(std::move(response));
  }
}

