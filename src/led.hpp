#pragma once

#include <cstdint>

namespace hhctrl::hw
{
  class LedDriver
  {
  public:
    virtual ~LedDriver() = default;
    virtual void set_brightness(uint8_t) const = 0;
  };
}