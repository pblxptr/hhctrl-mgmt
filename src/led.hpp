#pragma once

namespace hhctrl::hw
{
  class LedDriver
  {
  public:
    virtual ~Led() = default;
    virtual void turn_on() const = 0;
    virtual void turn_off() const = 0;
  };
}