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
    static constexpr char* name { "change_position" };
    static constexpr char* open { "open" };
    static constexpr char* close { "close" };
  };
  struct SlowStartAttr
  {
    static constexpr char* name { "slow_start" };
    static constexpr char* enable { "1" };
    static constexpr char* disable { "0" };
  };

  constexpr auto StatusMapping = StaticMap<HatchStatus, std::string_view, 5> {
    std::pair(HatchStatus::Open, "open"sv),
    std::pair(HatchStatus::Closed, "closed"sv),
    std::pair(HatchStatus::ChangingPosition, "changing_position"sv),
    std::pair(HatchStatus::Faulty, "faulty"sv),
    std::pair(HatchStatus::Undefined, "undefined"sv)
  };

  template<class TRoot, class TPath>
  auto make_path(const TRoot& root, const TPath& path)
  {
    return root /= path;
  }
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
  sysfs::write_attr(get_path(ChangePositionAttr::name)), ChangePositionAttr::open);
}

void SysfsHatch::close() const
{
  sysfs::write_attr(get_path(ChangePositionAttr::name), ChangePositionAttr::close);
}

HatchStatus SysfsHatch::status() const
{
  const auto attr_val = sysfs::read_attr(get_path(StatusAttrName));

  return StatusMapping.at(attr_val.data());
}
}