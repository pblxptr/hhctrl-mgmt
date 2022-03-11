#pragma once

#include <hw/drivers/led/rgb3led.hpp>

#include <spdlog/spdlog.h>
#include <string_view>

namespace hw::platform_device
{
  class RGBLedDriverLoader
  {
  public:
    using Compatible_t = hw::drivers::RGBLedDriver;

    constexpr static std::string_view compatible()
    {
      return "rgb3led";
    }

    template<class Context>
    static Compatible_t* probe(Context& ctx, const PdTreeObject_t& object)
    {
      spdlog::get("hw")->debug("RGBLedDriverLoader: probe");

      const auto& leds = object.at("leds").as_array();
      auto red = load_led<hw::drivers::LedDriver>(ctx, leds, "red");
      auto green = load_led<hw::drivers::LedDriver>(ctx, leds, "green");
      auto blue = load_led<hw::drivers::LedDriver>(ctx, leds, "blue");

      if (not red || not green || not blue) {
        spdlog::get("hw")->error("Driver cannot be loaded. One of its dependecies is not configured properly");
        return nullptr;
      }

      return ctx.template register_device(std::make_unique<hw::drivers::RGB3LedDriver>(*red, *green, *blue));
    }
  private:
    template<class LedDriverInterface, class Context>
    static LedDriverInterface* load_led(Context& ctx, const PdTreeArray_t& leds, std::string_view led_label)
    {
      constexpr auto color_attr = "color";
      auto led_desc_it = std::find_if(leds.begin(), leds.end(), [&led_label](const auto& e) {
        const auto& e_obj = e.as_object();
        return e_obj.contains(color_attr) && pdtree_to_string(e_obj.at(color_attr)) == led_label;
      });

      if (led_desc_it == leds.end()) {
        spdlog::get("hw")->error("Cannot find led descriptor for label: {}", led_label);
        return nullptr;
      }
      return ctx.template probe<LedDriverInterface>(led_desc_it->as_object());
    }
  };
}