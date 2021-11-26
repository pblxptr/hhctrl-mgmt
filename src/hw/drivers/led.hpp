#pragma once

#include <cstdint>

namespace hw::drivers
{
  class LedDriver
  {
  public:
    virtual ~LedDriver() = default;
    virtual void set_brightness(uint8_t) const = 0;
  };
}