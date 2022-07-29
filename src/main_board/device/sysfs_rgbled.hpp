#pragma once

#include <device/rgb_led.hpp>
#include <main_board/device/sysfs_led.hpp>

namespace mgmt::device
{
  class SysfsRGBLed
  {
  public:
    SysfsRGBLed(SysfsLed red, SysfsLed green, SysfsLed blue);

    SysfsRGBLed(const SysfsRGBLed&) = delete;
    SysfsRGBLed& operator=(const SysfsRGBLed&) = delete;

    SysfsRGBLed(SysfsRGBLed&&) noexcept = default;
    SysfsRGBLed& operator=(SysfsRGBLed&&) noexcept = default;

    RGBLedValue brightness() const;
    void set_brightness(const RGBLedValue& value);
  private:
    SysfsLed red_;
    SysfsLed green_;
    SysfsLed blue_;
  };
}