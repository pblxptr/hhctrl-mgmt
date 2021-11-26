#include "rgb_led_service.hpp"

#include <array>

#include <common/utils/static_map.hpp>


using namespace common::utils;

namespace {

  struct RGBbrightnessProxy
  {
    constexpr static uint8_t NumberOfLeds = 3;
    template<class...TLeds>
    constexpr void enable(const TLeds&...led) const
    {
      static_assert(NumberOfLeds== sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

      size_t i = 0;
      (led.set_brightness(data_[i++]), ...);
    }

    template<class...TLeds>
    constexpr void disable(const TLeds&...led) const
    {
      static_assert(NumberOfLeds == sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

      (led.set_brightness(0), ...);
    }

    template<class...TLeds>
    constexpr bool is_enabled(const TLeds&...led) const
    {
      size_t i = 0;
      return (... && (led.get_brightness() == data_[i++]) );
    }
    std::array<uint8_t, NumberOfLeds> data_;
  };

  constexpr auto IndicatorMapping = StaticMap<hw::services::IndicatorType, RGBbrightnessProxy, 4> {
    std::pair(hw::services::IndicatorType::Status, RGBbrightnessProxy{0, 255, 0}),
    std::pair(hw::services::IndicatorType::Warning, RGBbrightnessProxy{255, 255, 0}),
    std::pair(hw::services::IndicatorType::Maintenance, RGBbrightnessProxy{0, 0, 255}),
    std::pair(hw::services::IndicatorType::Fault, RGBbrightnessProxy{255, 0, 0})
  };

}

namespace hw::services
{
RgbLedService::RgbLedService(hw::drivers::LedDriver& red, hw::drivers::LedDriver& green, hw::drivers::LedDriver& blue)
  : red_{red}
  , green_{green}
  , blue_{blue}
{}

void RgbLedService::set_state(IndicatorType indicator, IndicatorState state) const
{
  const auto& led_proxy = IndicatorMapping.at(indicator);

  switch (state)
  {
    case IndicatorState::SteadyOn:
      led_proxy.enable(red_, green_, blue_);
      break;
    case IndicatorState::SteadyOff:
      led_proxy.disable(red_, green_, blue_);
      break;
  }
}

IndicatorState RgbLedService::get_state(IndicatorType indicator) const
{
  const auto& led_proxy = IndicatorMapping.at(indicator);

  if (led_proxy.is_enabled()) {
    return IndicatorState::SteadyOn;
  } else {
    return IndicatorState::SteadyOff;
  }
}
}