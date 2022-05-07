#pragma once

#include <new/device/led/rgb_led.hpp>
#include <new/device/led/led.hpp>

namespace mgmt::device
  class RGB3Led : public RGBLed
{
  {
  public:
    RGB3Led(
      DeviceId_t,
      std::unique_ptr<mgmt::device::Led>,
      std::unique_ptr<mgmt::device::Led>,
      std::unique_ptr<mgmt::device::Led>
    );

    void set_brightness(const RGBLed::Value&) override;
  private:
    std::unique_ptr<mgmt::device::Led> red_;
    std::unique_ptr<mgmt::device::Led> green_;
    std::unique_ptr<mgmt::device::Led> blue_;
  };
}