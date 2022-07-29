#include <device/rgb_indicator.hpp>

#include <common/utils/static_map.hpp>
#include <device/logger.hpp>

namespace {
  using namespace mgmt::device;

  struct RGBbrightnessProxy
  {
    uint8_t red {};
    uint8_t green {};
    uint8_t blue {};
  };

  constexpr auto IndicatorMapping = common::utils::StaticMap<IndicatorType, RGBbrightnessProxy, 4> {
    std::pair(IndicatorType::Status, RGBbrightnessProxy{0, 255, 0}),
    std::pair(IndicatorType::Warning, RGBbrightnessProxy{255, 255, 0}),
    std::pair(IndicatorType::Maintenance, RGBbrightnessProxy{0, 0, 255}),
    std::pair(IndicatorType::Fault, RGBbrightnessProxy{255, 0, 0})
  };
}

namespace mgmt::device
{
  RGBIndicator::RGBIndicator(IndicatorType type, std::shared_ptr<RGBLed_t> rgbled)
    : type_{type}
    , rgbled_{std::move(rgbled)}
  {}

  IndicatorType RGBIndicator::type() const
  {
    return type_;
  }

  IndicatorState RGBIndicator::state() const
  {
    const auto brightness = rgbled_->brightness();
    const auto proxy_led_brightness = IndicatorMapping.at(type_);

    if (proxy_led_brightness.red == brightness.red
        && proxy_led_brightness.green == brightness.green
        && proxy_led_brightness.blue == brightness.blue
    ){
      return IndicatorState::On;
    }
    else {
      return IndicatorState::Off;
    }
  }

  void RGBIndicator::set_state(IndicatorState state)
  {
    common::logger::get(mgmt::device::Logger)->debug("RGBIndicator::{}, indicator: {}, state: {}", __FUNCTION__, to_string(type_), to_string(state));

    const auto& config = IndicatorMapping.at(type_);

      switch (state)
      {
        case IndicatorState::On:
          rgbled_->set_brightness({
            .red = config.red,
            .green = config.green,
            .blue = config.blue
          });
          break;
        case IndicatorState::Off:
          rgbled_->set_brightness({
            .red = 0,
            .green = 0,
            .blue = 0
          });
          break;
        case IndicatorState::NotAvailable:
        case IndicatorState::Blinking:
          common::logger::get(mgmt::device::Logger)->debug("State not applicable");
      }
  }
}