#pragma once

#include <spdlog/spdlog.h>

#include <hw/platform_device/driver_loader.hpp>
#include <hw/drivers/led/sysfs_led.hpp>

namespace hw::platform_device
{
class LedDriverLoader : public BaseDriverLoader<hw::drivers::SysfsLedDriver>
{
public:
  constexpr std::string_view compatible() const override
  {
    return "sysfs_led";
  }
  hw::drivers::SysfsLedDriver* probe(const PdTreeObject_t& object) override
  {
    spdlog::get("hw")->info("LedDriverLoader: probe");

    if (not object.contains("sysfs_path")) {
      spdlog::get("hw")->error("Missing attribute 'sysfs_path' id pdtree for led driver descriptor");
      return nullptr;
    }

    // return std::make_unique<hw::drivers::SysfsLedDriver>(object.at("sysfs_path").as_string().c_str());
    return nullptr;
  }
};
}