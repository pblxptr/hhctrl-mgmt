#include "rgb_led_service.hpp"

#include <array>

namespace {
template<auto...Value>
struct RGBbrightnessProxy
{
  constexpr static std::size_t NumberOfValues = sizeof...(Value);

  template<class...TLeds>
  static constexpr void enable(const TLeds&...led)
  {
    static_assert(NumberOfValues == sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

    size_t i = 0;
    (led.set_brightness(v[i++]), ...);
  }

  template<class...TLeds>
  static constexpr void disable(const TLeds&...led)
  {
    static_assert(NumberOfValues == sizeof...(TLeds), "Leds number passed to set does not match to brightness config.");

    (led.set_brightness(0), ...);
  }

  template<class...TLeds>
  static constexpr bool is_enabled(const TLeds&...led)
  {
    size_t i = 0;
    return (... && (led.get_brightness() == v[i++]) );
  }

  static constexpr std::array<uint8_t, NumberOfValues> v {Value...};
};

  using StatusIndicatorRgbConfig_t =      RGBbrightnessProxy<0, 255, 0>;
  using WarningIndicatorRgbConfig_t =     RGBbrightnessProxy<255, 255, 0>;
  using MaintenanceIndicatorRgbConfig_t = RGBbrightnessProxy<0, 0, 255>;
  using FaultIndicatorRgbConfig_t =       RGBbrightnessProxy<255, 0, 0>;

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

}

IndicatorState RgbLedService::get_state(IndicatorType) const
{
  return {};
}

}