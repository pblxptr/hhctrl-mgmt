#pragma once

#include "indicator.hpp"

namespace hhctrl::hw
{
  class LedService
  {
  public:
    virtual ~LedService() = default;
    virtual void set_state(IndicatorType, IndicatorState) const = 0;
    virtual IndicatorState get_state(IndicatorType) const = 0;
  };
}