#include <newnew/sysfs_led.hpp>
#include <common/utils/sysfs.hpp>

#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace {
  namespace sysfs = common::utils::sysfs;
  struct BrightnessAttr
  {
    static constexpr const char* name { "brightness" };
  };
}

namespace mgmt::device
{
SysfsLed::SysfsLed(DeviceId_t device_id, std::string sysfsdir)
  : Led(std::move(device_id))
  , sysfsdir_{common::utils::sysfs::get_path(sysfsdir)}
{}

void SysfsLed::set_brightness(uint8_t value)
{
  const auto path = sysfsdir_ / BrightnessAttr::name;
  sysfs::write_attr(path, static_cast<int>(value));
}
}
