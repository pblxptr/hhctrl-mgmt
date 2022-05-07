#pragma once

#include <hw/drivers/led/rgb_led.hpp>
#include <hw/drivers/led/led.hpp>
#include <memory>
#include <string>

namespace hw::drivers
{
  class RGB3LedDriver : public RGBLedDriver
  {
  public:
    RGB3LedDriver(
      hw::drivers::LedDriver&,
      hw::drivers::LedDriver&,
      hw::drivers::LedDriver&
    );

    void set_brightness(const RGBLedDriver::Value&) override;
  private:
    hw::drivers::LedDriver& red_;
    hw::drivers::LedDriver& green_;
    hw::drivers::LedDriver& blue_;
  };
}