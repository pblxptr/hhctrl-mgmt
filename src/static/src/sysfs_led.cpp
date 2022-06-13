#include <static/device/sysfs_led.hpp>

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
SysfsLed::SysfsLed(std::string sysfsdir)
  : sysfsdir_{common::utils::sysfs::get_path(sysfsdir)}
{
  if (not fs::exists(sysfsdir_)) {
    spdlog::get("mgmt")->error("Directory {} does not exist.", sysfsdir_.c_str());
  }
}

void SysfsLed::set_brightness(int value)
{
  const auto path = sysfsdir_ / BrightnessAttr::name;
  sysfs::write_attr(path, static_cast<int>(value));
}
}