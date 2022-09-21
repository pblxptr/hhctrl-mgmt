#include <main_board/device/sysfs_led.hpp>

#include <utils/sysfs.hpp>
#include <device/logger.hpp>

namespace {
namespace sysfs = common::utils::sysfs;
struct BrightnessAttr
{
  static constexpr const char* name{ "brightness" };
};
}// namespace

namespace mgmt::device {
SysfsLed::SysfsLed(const std::string& sysfsdir)
  : sysfsdir_{ common::utils::sysfs::get_path(sysfsdir) }
{}

int SysfsLed::brightness() const
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsLed::{}", __FUNCTION__);

  const auto path = sysfsdir_ / BrightnessAttr::name;
  return std::stoi(sysfs::read_attr(path));
}

void SysfsLed::set_brightness(int value)
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsLed::{}", __FUNCTION__);

  const auto path = sysfsdir_ / BrightnessAttr::name;
  sysfs::write_attr(path, static_cast<int>(value));
}

}// namespace mgmt::device
