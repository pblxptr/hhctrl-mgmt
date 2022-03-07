#include <hw/drivers/led/sysfs_led.hpp>
#include <hw/drivers/sysfs.hpp>

#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace {
  namespace sysfs = hhctrl::helpers::sysfs;
  struct BrightnessAttr
  {
    static constexpr const char* name { "brightness" };
  };
}

namespace hw::drivers
{
SysfsLedDriver::SysfsLedDriver(std::string sysfsdir)
{
  if (!fs::exists(sysfsdir))
  {
    throw std::runtime_error(fmt::format("Sysfs directory: {} for SysfsLedDriver does not exist", sysfsdir));
  }
  sysfsdir_ = std::move(sysfsdir);

  spdlog::get("hw")->debug("SysfsLedDriver has been loaded successfully");
}

void SysfsLedDriver::set_brightness(uint8_t value) const
{
  const auto path = sysfsdir_ / BrightnessAttr::name;
  sysfs::write_attr(path, static_cast<int>(value));
}
}
