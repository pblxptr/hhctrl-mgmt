//
// Created by pp on 7/30/22.
//
#include <main_board/device/sysfs_ds18b20.hpp>

#include <common/utils/sysfs.hpp>
#include <device/logger.hpp>

namespace {
namespace sysfs = common::utils::sysfs;

  constexpr auto ValueAttr = "w1_slave";
}// namespace

namespace mgmt::device {
SysfsDS18B20::SysfsDS18B20(std::string sysfsdir)
  : sysfsdir_{ common::utils::sysfs::get_path(sysfsdir) }
{
  if (not fs::exists(sysfsdir_)) {
    common::logger::get(mgmt::device::Logger)->error("Directory {} does not exist.", sysfsdir_.c_str());
  }
}

float SysfsDS18B20::value() const
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsLed::{}", __FUNCTION__);

  const auto attr_val = sysfs::read_attr(sysfsdir_ / ValueAttr);
  const auto pos = attr_val.find("t=");

  if (pos == std::string::npos) {
    common::logger::get(mgmt::device::Logger)->error("Invalid data for temperature sensor");

    return 0;
  }

  return atof(attr_val.data() + pos + 2) / 1000; //output + position where "t=" starts, skip "t="
}

}// namespace mgmt::device