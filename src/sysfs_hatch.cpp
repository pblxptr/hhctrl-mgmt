#include "sysfs_hatch.hpp"

#include <filesystem>
#include <string_view>
#include <fstream>
#include <tuple>

#include "static_map.hpp"
#include "sysfs.hpp"

using namespace utils;
using namespace hhctrl::hw;
using namespace std::literals;

namespace sysfs = hhctrl::helpers::sysfs;
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

  constexpr auto StatusMapping = StaticMap<HatchStatus, std::string_view, 5> {
    std::pair(HatchStatus::Open, "open"sv),
    std::pair(HatchStatus::Closed, "closed"sv),
    std::pair(HatchStatus::ChangingPosition, "changing_position"sv),
    std::pair(HatchStatus::Faulty, "faulty"sv),
    std::pair(HatchStatus::Undefined, "undefined"sv)
  };
}

namespace hhctrl::hw
{
SysfsHatch::SysfsHatch(std::string sysfsdir)
{
  if (!fs::exists(sysfsdir))
  {
    throw std::runtime_error("Path does not exist.");
  }
  sysfsdir_ = std::move(sysfsdir);
}

void SysfsHatch::open() const
{
  sysfs::write_attr(get_path(ChangePositionAttr::name), ChangePositionAttr::open);
}

void SysfsHatch::close() const
{
  sysfs::write_attr(get_path(ChangePositionAttr::name), ChangePositionAttr::close);
}

HatchStatus SysfsHatch::status() const
{
  const auto attr_val = sysfs::read_attr(get_path(StatusAttr::name));

  return StatusMapping.at(attr_val.data());
}
}