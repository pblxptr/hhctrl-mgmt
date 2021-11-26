#pragma once

#include <hw/drivers/led.hpp>

#include <string>
#include <filesystem>

namespace hw::drivers
{
  class SysfsLed : public LedDriver
  {
  public:
    explicit SysfsLed(std::string);

    virtual void set_brightness(uint8_t) const;
  private:
    template<class TPath>
    auto get_path(const TPath& path) const { return sysfsdir_ / path; }
  private:
    std::filesystem::path sysfsdir_;
  };
}