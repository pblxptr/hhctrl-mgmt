#pragma once

#include <static/device/indicator_type.hpp>
#include <static/device/indicator_state.hpp>
#include <static/device/rgb_led.hpp>

namespace mgmt::device
{
  class RGBIndicator
  {
  public:
    RGBIndicator(IndicatorType, std::shared_ptr<RGBLed_t>);
    IndicatorType type() const;
    IndicatorState state() const;
    void set_state(IndicatorState);

  private:
    IndicatorType type_;
    std::shared_ptr<RGBLed_t> rgbled_;
  };
}