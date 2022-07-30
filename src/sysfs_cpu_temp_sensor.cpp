//
// Created by pp on 7/30/22.
//
#include <main_board/device/sysfs_cpu_temp_sensor.hpp>

#include <common/utils/sysfs.hpp>
#include <device/logger.hpp>

namespace {
namespace sysfs = common::utils::sysfs;

  constexpr auto ValueAttr = "temp";
}// namespace

namespace mgmt::device {
SysfsCPUTempSensor::SysfsCPUTempSensor(std::string sysfsdir)
  : sysfsdir_{ common::utils::sysfs::get_path(sysfsdir) }
{
  if (not fs::exists(sysfsdir_)) {
    common::logger::get(mgmt::device::Logger)->error("Directory {} does not exist.", sysfsdir_.c_str());
  }
}

float SysfsCPUTempSensor::value() const
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsCPUTempSensor::{}", __FUNCTION__);

  const auto attr_val = sysfs::read_attr(sysfsdir_ / ValueAttr);

  return atof(attr_val.data()) / 1000;
}

}// namespace mgmt::device