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
  static constexpr const char* Name{ "change_position" };
  static constexpr const char* Open{ "open" };
  static constexpr const char* Close{ "close" };
};

struct StatusAttr
{
  static constexpr const char* Name{ "status" };
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
  common::logger::get(mgmt::device::Logger)->debug("SysfsHatch::{}", __FUNCTION__);

  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::Name, ChangePositionAttr::Open);
}

void SysfsHatch::close() const
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsHatch::{}", __FUNCTION__);

  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::Name, ChangePositionAttr::Close);
}

HatchState SysfsHatch::status() const
{
  common::logger::get(mgmt::device::Logger)->debug("SysfsHatch::{}", __FUNCTION__);

  const auto attr_val = sysfs::read_attr(sysfsdir_ / StatusAttr::Name);

  return StatusMapping.at(attr_val.data());
}
}// namespace mgmt::device
