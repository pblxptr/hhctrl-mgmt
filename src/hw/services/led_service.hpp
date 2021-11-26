#pragma once

#include "indicator.hpp"

namespace hw::services
{
  class LedService
  {
  public:
    virtual ~LedService() = default;
    virtual void set_state(IndicatorType, IndicatorState) const = 0;
    virtual IndicatorState get_state(IndicatorType) const = 0;
  };
}