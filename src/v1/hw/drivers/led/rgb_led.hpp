#pragma once

#include <hw/drivers/driver.hpp>

#include <cstdint>

namespace hw::drivers
{
  class RGBLedDriver : public Driver
  {
  public:
    struct Value
    {
      const uint8_t red;
      const uint8_t green;
      const uint8_t blue;
    };
    virtual void set_brightness(const Value&) = 0;
  };
}
