#pragma once

#include <hw/drivers/driver.hpp>

#include <cstdint>

namespace hw::drivers
{
  class LedDriver : public Driver
  {
  public:
    virtual void set_brightness(uint8_t) const = 0;
  };
}