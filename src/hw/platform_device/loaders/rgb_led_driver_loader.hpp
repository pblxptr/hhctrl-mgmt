#pragma once

#include <spdlog/spdlog.h>

#include <hw/platform_device/driver_loader.hpp>
#include <hw/drivers/led/rgb3led.hpp>
#include <memory>

namespace hw::platform_device
{
class RGBLedDriverLoader : public BaseDriverLoader<hw::drivers::RGB3LedDriver>
{
public:
  constexpr std::string_view compatible() const override
  {
    return "rgb3led";
  }
  void probe(const PdTreeObject_t& object) override
  {
    spdlog::get("hw")->info("RGBLedDriverLoader: probe");
    const auto& leds = object.at("leds").as_array();
    auto red = load_led(pd_loader, leds, "red");
    auto green = load_led(pd_loader, leds, "green");
    auto blue = load_led(pd_loader, leds, "blue");

    if (not red || not green || not blue) {
      spdlog::get("hw")->error("Driver cannot be loaded. One of its dependecies is not configured properly");
      return nullptr;
    }

    return std::make_unique<hw::drivers::RGB3LedDriver>(
      std::move(red),
      std::move(green),
      std::move(blue)
    );
  }
private:
  std::unique_ptr<hw::drivers::LedDriver> load_led(const PlatformDeviceLoader& pd_loader, const PdTreeArray_t& leds, std::string_view led_label)
  {
    auto led_desc_it = std::find_if(leds.begin(), leds.end(), [&led_label](const auto& e) {
      const auto& e_obj = e.as_object();
      return e_obj.contains("label") && e_obj.at("label").as_string().c_str() == led_label;
    });

    if (led_desc_it == leds.end()) {
      spdlog::get("hw")->error("Cannot find led descriptor for label: {}", led_label);
      return nullptr;
    }

    auto loader = pd_loader.find_loader(led_desc_it->as_object());
    if (!loader) {
      return nullptr;
    }

    auto driver = loader->probe(pd_loader, led_desc_it->as_object());
    if (!driver) {
      return nullptr;
    }

    return std::unique_ptr<hw::drivers::LedDriver>(
      static_cast<hw::drivers::LedDriver*>(driver.release())
    );
  }
};
}
