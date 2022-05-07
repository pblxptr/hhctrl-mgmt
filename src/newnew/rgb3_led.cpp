#include <newnew/rgb3_led.hpp>

#include <spdlog/spdlog.h>

namespace mgmt::device
{
  RGB3Led::RGB3Led(
    DeviceId_t device_id,
    std::unique_ptr<mgmt::device::Led> red,
    std::unique_ptr<mgmt::device::Led> green,
    std::unique_ptr<mgmt::device::Led> blue
  )
  : RGBLed{std::move(device_id)}
  , red_{std::move(red)}
  , green_{std::move(green)}
  , blue_{std::move(blue)}
  { }

  void RGB3Led::set_brightness(const RGBLed::Value& value)
  {
    red_->set_brightness(value.red);
    green_->set_brightness(value.green);
    blue_->set_brightness(value.blue);
  }
}