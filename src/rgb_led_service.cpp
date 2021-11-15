#include "rgb_led_service.hpp"
#include "static_map.hpp"

#include <array>

using namespace hhctrl::utils;

namespace {
// template<auto...Value>
// struct RGBbrightnessProxy
// {
//   constexpr static std::size_t NumberOfValues = sizeof...(Value);

//   template<class...TLeds>
//   static constexpr void enable(const TLeds&...led)
//   {
//     static_assert(NumberOfValues == sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

//     size_t i = 0;
//     (led.set_brightness(v[i++]), ...);
//   }

//   template<class...TLeds>
//   static constexpr void disable(const TLeds&...led)
//   {
//     static_assert(NumberOfValues == sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

//     (led.set_brightness(0), ...);
//   }

//   template<class...TLeds>
//   static constexpr bool is_enabled(const TLeds&...led)
//   {
//     size_t i = 0;
//     return (... && (led.get_brightness() == v[i++]) );
//   }

//   static constexpr std::array<uint8_t, NumberOfValues> v {Value...};
// };

  // using StatusIndicatorRgbConfig_t =      RGBbrightnessProxy<0, 255, 0>;
  // using WarningIndicatorRgbConfig_t =     RGBbrightnessProxy<255, 255, 0>;
  // using MaintenanceIndicatorRgbConfig_t = RGBbrightnessProxy<0, 0, 255>;
  // using FaultIndicatorRgbConfig_t =       RGBbrightnessProxy<255, 0, 0>;

  struct RGBbrightnessProxy2
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

  constexpr auto IndicatorMapping = StaticMap<hhctrl::hw::IndicatorType, RGBbrightnessProxy2, 4> {
    std::pair(hhctrl::hw::IndicatorType::Status, RGBbrightnessProxy2{0, 255, 0}),
    std::pair(hhctrl::hw::IndicatorType::Warning, RGBbrightnessProxy2{255, 255, 0}),
    std::pair(hhctrl::hw::IndicatorType::Maintenance, RGBbrightnessProxy2{0, 0, 255}),
    std::pair(hhctrl::hw::IndicatorType::Fault, RGBbrightnessProxy2{255, 0, 0})
  };

}

namespace hhctrl::hw
{
RgbLedService::RgbLedService(LedDriver& red, LedDriver& green, LedDriver& blue)
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