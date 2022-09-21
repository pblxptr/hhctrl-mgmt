#include <main_board/device/sysfs_hatch.hpp>

#include <filesystem>
#include <string_view>

#include <device/logger.hpp>
#include <utils/static_map.hpp>
#include <utils/sysfs.hpp>

using namespace std::literals;

namespace sysfs = common::utils::sysfs;

namespace {
struct ChangePositionAttr
{
  static constexpr const char* name{ "change_position" };
  static constexpr const char* open{ "open" };
  static constexpr const char* close{ "close" };
};

struct StatusAttr
{
  static constexpr const char* name{ "status" };
};

constexpr auto StatusMapping = common::utils::StaticMap<mgmt::device::HatchState, std::string_view, 5>{
  std::pair(mgmt::device::HatchState::Open, "open"sv),
  std::pair(mgmt::device::HatchState::Closed, "closed"sv),
  std::pair(mgmt::device::HatchState::ChangingPosition, "changing_position"sv),
  std::pair(mgmt::device::HatchState::Faulty, "faulty"sv),
  std::pair(mgmt::device::HatchState::Undefined, "undefined"sv)
};
}// namespace

namespace mgmt::device {
SysfsHatch::SysfsHatch(const std::string& sysfsdir)
  : sysfsdir_{ sysfs::get_path(sysfsdir) }
{}

void SysfsHatch::open() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::open);
}

void SysfsHatch::close() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::close);
}

HatchState SysfsHatch::status() const
{
  const auto attr_val = sysfs::read_attr(sysfsdir_ / StatusAttr::name);

  return StatusMapping.at(attr_val.data());
}
}// namespace mgmt::device
