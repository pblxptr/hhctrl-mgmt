#pragma once

#include "../../../../device/include/device/rgb_led.hpp"
#include "sysfs_led.hpp"

namespace mgmt::device {
class SysfsRGBLed
{
public:
  SysfsRGBLed(SysfsLed red, SysfsLed green, SysfsLed blue);
  // movable
  SysfsRGBLed(const SysfsRGBLed&) = delete;
  SysfsRGBLed& operator=(const SysfsRGBLed&) = delete;
  // non-copyable
  SysfsRGBLed(SysfsRGBLed&&) noexcept = default;
  SysfsRGBLed& operator=(SysfsRGBLed&&) noexcept = default;

  ~SysfsRGBLed() = default;

  RGBLedValue brightness() const;
  void set_brightness(const RGBLedValue& value);

private:
  SysfsLed red_;
  SysfsLed green_;
  SysfsLed blue_;
};
}// namespace mgmt::device
