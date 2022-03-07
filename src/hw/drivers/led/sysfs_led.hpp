#pragma once

#include <hw/drivers/led/led.hpp>

#include <string>
#include <filesystem>

namespace hw::drivers
{
  class SysfsLedDriver : public LedDriver
  {
  public:
    explicit SysfsLedDriver(std::string);

    virtual void set_brightness(uint8_t) const;
  private:
    std::filesystem::path sysfsdir_;
  };
}