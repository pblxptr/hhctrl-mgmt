#pragma once

namespace hhctrl::hw
{
  enum class IndicatorType { Status, Warning, Maintenance, Fault };
  enum class IndicatorState { SteadyOn, SteadyOff };

  class LedService
  {
  public:
    virtual ~LedService() = default;
    virtual void set_state(IndicatorType, IndicatorState) const = 0;
    virtual IndicatorState get_state(IndicatorType) const = 0;
  };
}