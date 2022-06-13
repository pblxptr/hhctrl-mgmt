#include <static/device/sysfs_hatch.hpp>

#include <filesystem>
#include <string_view>
#include <fstream>
#include <tuple>
#include <spdlog/spdlog.h>

#include <common/utils/static_map.hpp>
#include <common/utils/sysfs.hpp>

using namespace common::utils;
using namespace std::literals;

namespace sysfs = common::utils::sysfs;
namespace fs = std::filesystem;

namespace {
  struct ChangePositionAttr
  {
    static constexpr const char* name { "change_position" };
    static constexpr const char* open { "open" };
    static constexpr const char* close { "close" };
  };
  struct SlowStartAttr
  {
    static constexpr const char* name { "slow_start" };
    static constexpr const char* enable { "1" };
    static constexpr const char* disable { "0" };
  };

  struct StatusAttr
  {
    static constexpr const char* name { "status" };
  };

  constexpr auto StatusMapping = StaticMap<mgmt::device::HatchStatus, std::string_view, 5> {
    std::pair(mgmt::device::HatchStatus::Open, "open"sv),
    std::pair(mgmt::device::HatchStatus::Closed, "closed"sv),
    std::pair(mgmt::device::HatchStatus::ChangingPosition, "changing_position"sv),
    std::pair(mgmt::device::HatchStatus::Faulty, "faulty"sv),
    std::pair(mgmt::device::HatchStatus::Undefined, "undefined"sv)
  };
}

namespace mgmt::device
{
SysfsHatch::SysfsHatch(std::string sysfsdir)
  : sysfsdir_{common::utils::sysfs::get_path(sysfsdir)}
{
  if (not fs::exists(sysfsdir_)) {
    spdlog::get("mgmt")->error("Directory {} does not exist.", sysfsdir_.c_str());
  }
}

void SysfsHatch::open() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::open);
}

void SysfsHatch::close() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::close);
}

HatchStatus SysfsHatch::status() const
{
  const auto attr_val = sysfs::read_attr(sysfsdir_ / StatusAttr::name);

  return StatusMapping.at(attr_val.data());
}
}