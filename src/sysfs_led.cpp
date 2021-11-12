#include "sysfs_led.hpp"
#include "sysfs.hpp"

namespace {
  struct BrightnessAttr
  {
    static constexpr char* name { "brightness" };
    static constexpr char* min { "0" };
    static constexpr char* max { "1" };
  };
}

namespace hhctrl::hw

SysfsLed::SysfsLed(std::string sysfsdir)
{
  if (!fs::exists(sysfsdir))
  {
    throw std::runtime_error("Path does not exist.");
  }
  sysfsdir_ = std::move(sysfsdir);
}

void SysfsLed::turn_on() const
{
  sysfs::write_attr(get_path(BrightnessAttr::name)), BrightnessAttr::max);
}

void SysfsLed::turn_off() const
{
  sysfs::write_attr(get_path(BrightnessAttr::name), BrightnessAttr::min);
}