#pragma once

#include "pdtree.hpp"

#include <device/indicator_t.hpp>
#include <device/rgb_led.hpp>

namespace mgmt::platform_device {
class RGBIndicatorProvider
{
public:
  constexpr static const char* compatible()
  {
    return "sysfs_rgbled_indicator";
  }

  template<class BoardBuilder>
  bool load(BoardBuilder& builder, const PdTreeObject_t& object)
  {
    const auto& leds = object.at("leds").as_array();
    auto red = load_led(leds, "red");
    auto green = load_led(leds, "green");
    auto blue = load_led(leds, "blue");

    if (not red || not green || not blue) {
      spdlog::get("hw")->debug("Driver cannot be loaded. One of its dependencies is not configured properly");
      return false;
    }

    auto rgb_led = std::make_shared<mgmt::device::RGBLed_t>(
      std::move(*red),
      std::move(*green),
      std::move(*blue));
    builder.add_device(mgmt::device::Indicator_t{ mgmt::device::RGBIndicator{
      mgmt::device::IndicatorType::Fault,
      rgb_led } });
    builder.add_device(mgmt::device::Indicator_t{ mgmt::device::RGBIndicator{
      mgmt::device::IndicatorType::Maintenance,
      rgb_led } });
    builder.add_device(mgmt::device::Indicator_t{ mgmt::device::RGBIndicator{
      mgmt::device::IndicatorType::Status,
      rgb_led } });
    builder.add_device(mgmt::device::Indicator_t{ mgmt::device::RGBIndicator{
      mgmt::device::IndicatorType::Warning,
      rgb_led } });

    return true;
  }

private:
  static std::optional<mgmt::device::SysfsLed> load_led(const PdTreeArray_t& leds, std::string_view led_label)
  {
    constexpr auto sysfs_path_atrr = "sysfs_path";
    auto&& led_descriptor = std::find_if(leds.begin(), leds.end(), [&led_label](const auto& prop) {
      const auto& e_obj = prop.as_object();
      return e_obj.contains("color") && pdtree_to_string(e_obj.at("color")) == led_label;
    });

    if (led_descriptor == leds.end()) {
      common::logger::get(mgmt::device::Logger)->debug("Cannot find led descriptor for label: {}", led_label);
      return std::nullopt;
    }

    const auto descriptor_obj = led_descriptor->as_object();
    if (not descriptor_obj.contains(sysfs_path_atrr)) {
      common::logger::get(mgmt::device::Logger)->error("Missing attribute 'sysfs_path' in pdtree for led driver descriptor");
      return std::nullopt;
    }
    return mgmt::device::SysfsLed{ pdtree_to_string(descriptor_obj.at(sysfs_path_atrr)) };
  }
};
}// namespace mgmt::platform_device
