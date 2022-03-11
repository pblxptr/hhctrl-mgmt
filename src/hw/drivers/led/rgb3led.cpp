#include <hw/drivers/led/rgb3led.hpp>
#include <spdlog/spdlog.h>

namespace hw::drivers
{
  RGB3LedDriver::RGB3LedDriver(
    hw::drivers::LedDriver& red,
    hw::drivers::LedDriver& green,
    hw::drivers::LedDriver& blue
  )
  : red_{red}
  , green_{green}
  , blue_{blue}
  {
  }

  void RGB3LedDriver::set_brightness(const RGBLedDriver::Value& value)
  {
    red_.set_brightness(value.red);
    green_.set_brightness(value.green);
    blue_.set_brightness(value.blue);
  }
}