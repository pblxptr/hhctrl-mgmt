#include "sysfs_hatch.hpp"

#include <filesystem>
#include <string_view>
#include <fstream>
#include <tuple>

#include "static_map.hpp"

using namespace utils;
using namespace hhctrl::hw;
using namespace std::literals;

namespace fs = std::filesystem;


namespace {
  constexpr auto statusAttrName = "status"sv;
  constexpr auto changePositionAttrName = "change_position";
  constexpr auto slowStartAttrName = "slow_start";

  constexpr auto statusMap = StaticMap<HatchStatus, std::string_view, 5> {
    std::pair(HatchStatus::Open, "open"sv),
    std::pair(HatchStatus::Closed, "closed"sv),
    std::pair(HatchStatus::ChangingPosition, "changing_position"sv),
    std::pair(HatchStatus::Faulty, "faulty"sv),
    std::pair(HatchStatus::Undefined, "undefined"sv)
  };

  std::string read_attr(const fs::path& path)
  {
    auto ret = std::string{};
    auto fstream = std::fstream(path, std::ios::in);

    if (!fstream)
    {
      throw std::runtime_error{"Attribute does not exist."};
    }

    fstream >> ret;

    return ret;
  }

  template<class TValue>
  void write_attr(const fs::path& path, const TValue& val)
  {
    auto fstream = std::fstream(path, std::ios::out);

    if (!fstream)
    {
      throw std::runtime_error{"Attribute does not exist."};
    }

    fstream << val;
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
}

void SysfsHatch::open()
{
  write_attr(changePositionAttrName, "open");
}

void SysfsHatch::close()
{
  write_attr(changePositionAttrName, "close");
}

HatchStatus SysfsHatch::status() const
{
  const auto attr_val = read_attr(statusAttrName);

  return statusMap.at(attr_val.data());
}
}