#include "rgb_led_service.hpp"

#include <array>

#include <common/utils/static_map.hpp>
#include <spdlog/spdlog.h>


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

  constexpr auto IndicatorMapping = StaticMap<common::data::IndicatorType, RGBbrightnessProxy, 4> {
    std::pair(common::data::IndicatorType::Status, RGBbrightnessProxy{0, 255, 0}),
    std::pair(common::data::IndicatorType::Warning, RGBbrightnessProxy{255, 255, 0}),
    std::pair(common::data::IndicatorType::Maintenance, RGBbrightnessProxy{0, 0, 255}),
    std::pair(common::data::IndicatorType::Fault, RGBbrightnessProxy{255, 0, 0})
  };

}

namespace hw::services
{
RgbLedService::RgbLedService(hw::drivers::LedDriver& red, hw::drivers::LedDriver& green, hw::drivers::LedDriver& blue)
  : red_{red}
  , green_{green}
  , blue_{blue}
{}

void RgbLedService::set_state(common::data::IndicatorType indicator, common::data::IndicatorState state) const
{
  spdlog::get("hw")->info("RgbLedService: set_state");

  const auto& led_proxy = IndicatorMapping.at(indicator);

  switch (state)
  {
    case common::data::IndicatorState::SteadyOn:
      led_proxy.enable(red_, green_, blue_);
      break;
    case common::data::IndicatorState::SteadyOff:
      led_proxy.disable(red_, green_, blue_);
      break;
  }
}

common::data::IndicatorState RgbLedService::get_state(common::data::IndicatorType indicator) const
{
  spdlog::get("hw")->info("RgbLedService: get_state");

  const auto& led_proxy = IndicatorMapping.at(indicator);

  if (led_proxy.is_enabled()) {
    return common::data::IndicatorState::SteadyOn;
  } else {
    return common::data::IndicatorState::SteadyOff;
  }
}
}