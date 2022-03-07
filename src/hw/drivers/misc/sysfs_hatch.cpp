#include <hw/drivers/misc/sysfs_hatch.hpp>

#include <filesystem>
#include <string_view>
#include <fstream>
#include <tuple>
#include <spdlog/spdlog.h>

#include <common/utils/static_map.hpp>
#include <hw/drivers/sysfs.hpp>

using namespace common::utils;
using namespace hw::drivers;
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

namespace hw::drivers
{
SysfsHatchDriver::SysfsHatchDriver(std::string sysfsdir)
{
  if (!fs::exists(sysfsdir))
  {
    throw std::runtime_error(fmt::format("Sysfs directory: {} for SysfsHatchDriverDriver does not exist", sysfsdir));
  }
  sysfsdir_ = std::move(sysfsdir);

  spdlog::get("hw")->debug("SysfsHatchDriver has been loaded successfully");
}

void SysfsHatchDriver::open() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::open);
}

void SysfsHatchDriver::close() const
{
  sysfs::write_attr(sysfsdir_ / ChangePositionAttr::name, ChangePositionAttr::close);
}

HatchStatus SysfsHatchDriver::status() const
{
  const auto attr_val = sysfs::read_attr(sysfsdir_ / StatusAttr::name);

  return StatusMapping.at(attr_val.data());
}
}