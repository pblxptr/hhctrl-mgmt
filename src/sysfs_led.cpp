#include "sysfs_led.hpp"
#include "sysfs.hpp"

namespace {
  namespace sysfs = hhctrl::helpers::sysfs;
  struct BrightnessAttr
  {
    static constexpr const char* name { "brightness" };
  };
}

namespace hhctrl::hw
{
SysfsLed::SysfsLed(std::string sysfsdir)
{
  if (!fs::exists(sysfsdir))
  {
    throw std::runtime_error("Path does not exist.");
  }
  sysfsdir_ = std::move(sysfsdir);
}

void SysfsLed::set_brightness(uint8_t value) const
{
  sysfs::write_attr(get_path(BrightnessAttr::name), static_cast<int>(value));
}
}