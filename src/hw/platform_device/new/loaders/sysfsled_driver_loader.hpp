#pragma once

#include <hw/drivers/led/sysfs_led.hpp>

#include <spdlog/spdlog.h>
#include <string_view>

namespace hw::platform_device
{
  class SysfsLedDriverLoader
  {
  public:
    using Compatible_t = hw::drivers::LedDriver;

    constexpr static std::string_view compatible()
    {
      return "sysfs_led";
    }

    template<class Context>
    static Compatible_t* probe(Context& ctx, const PdTreeObject_t& object)
    {
      constexpr auto sysfs_path_atrr = "sysfs_path";
      constexpr auto color_attr = "color";

      spdlog::get("hw")->info("LedDriverLoader: probe");

      if (not object.contains(sysfs_path_atrr)) {
        spdlog::get("hw")->error("Missing attribute 'sysfs_path' in pdtree for led driver descriptor");
        return nullptr;
      }

      if (not object.contains(color_attr)) {
        spdlog::get("hw")->error("Missing attribute 'color' in pdtree for led driver descriptor");
        return nullptr;
      }

      return ctx.template register_device(
        std::make_unique<hw::drivers::SysfsLedDriver>(
            pdtree_to_string(object.at(sysfs_path_atrr))
        ),
        DeviceAttributes{std::pair{color_attr, pdtree_to_string(object.at(color_attr))}
      });
    }
  };
}