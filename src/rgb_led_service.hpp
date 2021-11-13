#pragma once

#include "led_service.hpp"
#include "led.hpp"

namespace hhctrl::hw
{
  class RgbLedService : public LedService
  {
  public:
    RgbLedService(LedDriver& rled, LedDriver& gled, LedDriver& bled);
    void set_state(IndicatorType, IndicatorState) const override;
    IndicatorState get_state(IndicatorType) const override;
  private:
    LedDriver& red_;
    LedDriver& green_;
    LedDriver& blue_;
  };
}