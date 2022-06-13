#pragma once

#include <static/device/indicator_type.hpp>
#include <static/device/indicator_state.hpp>
#include <static/device/rgb_led.hpp>

namespace mgmt::device
{
  class RGBIndicator
  {
  public:
    RGBIndicator(IndicatorType type, std::shared_ptr<RGBLed_t> rgbled)
      : type_{type}
      , rgbled_{std::move(rgbled)}
    {}

    constexpr IndicatorType type() const
    {
      return type_;
    }

    IndicatorState state() const
    {
      return IndicatorState::NotAvailable;
    }

    void set_state(IndicatorState state)
    {
      spdlog::get("mgmt")->debug("Setting state: {} on indicator: {}", to_string(state), to_string(type_));
    }

  private:
    IndicatorType type_;
    std::shared_ptr<RGBLed_t> rgbled_;
  };
}