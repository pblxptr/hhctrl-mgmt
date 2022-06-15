#pragma once

#include <main_board/device/sysfs_led.hpp>

namespace mgmt::device
{
  class SysfsRGBLed
  {
  public:
    struct Value {
      int red {};
      int green {};
      int blue {};
    };

    SysfsRGBLed(SysfsLed red, SysfsLed green, SysfsLed blue)
      : red_{std::move(red)}
      , green_{std::move(green)}
      , blue_{std::move(blue)}
    {}

    SysfsRGBLed(const SysfsRGBLed&) = delete;
    SysfsRGBLed& operator=(const SysfsRGBLed&) = delete;

    SysfsRGBLed(SysfsRGBLed&&) noexcept = default;
    SysfsRGBLed& operator=(SysfsRGBLed&&) noexcept = default;

    void set_brightness(const Value& value)
    {
      red_.set_brightness(value.red);
      green_.set_brightness(value.green);
      blue_.set_brightness(value.blue);
    }
  private:
    SysfsLed red_;
    SysfsLed green_;
    SysfsLed blue_;
  };
}