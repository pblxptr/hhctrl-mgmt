#pragma once

#include <filesystem>

#include <platform_device/pdtree.hpp>

namespace mgmt::platform_device
{
  template<class... Provider>
  class PlatformDeviceProvider //device discovery
  {
    using DeviceProvider_t = std::variant<Provider...>;
  public:
    template<class...P>
    PlatformDeviceProvider(std::string pdtree_file, P... providers)
      : pdtree_file_{std::move(pdtree_file)}
      , providers_{DeviceProvider_t{std::move(providers)}...}
    {
      //TODO: Uncomment
      // if (!fs::exists(path)) {
      //   throw std::runtime_error("Platform device tree file does not exist");
      // }
    }

    template<class BoardBuilder>
    void setup(BoardBuilder& builder)
    {
      auto pdtree_content = pdtree_load(pdtree_file_);

      for (const auto& entry : pdtree_content.as_array()) {
        const auto& descriptor = entry.as_object();
        const auto compatible = pdtree_to_string(descriptor.at("compatible"));
        auto provider = std::ranges::find_if(providers_, [&compatible](auto&& l) {
          auto call = [](auto&& l) { return l.compatible(); };

          return std::visit(call, l) == compatible;
        });

        if (provider == providers_.end()) {
          throw std::runtime_error(fmt::format("Cannot find loader for device with compatible: {}", compatible));
        }

        std::visit([&builder, &descriptor](auto&& p) { return p.load(builder, descriptor); }, *provider);
      }
    }
  private:
    std::string pdtree_file_;
    std::vector<DeviceProvider_t> providers_;
  };

  template<class...T>
  PlatformDeviceProvider(std::string, T...) -> PlatformDeviceProvider<T...>;
}