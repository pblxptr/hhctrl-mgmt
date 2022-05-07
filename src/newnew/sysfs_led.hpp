#pragma once

#include <newnew/led.hpp>

#include <string>
#include <filesystem>

namespace mgmt::device
{
  class SysfsLed : public Led
  {
  public:
    SysfsLed(DeviceId_t, std::string);

    void set_brightness(uint8_t) override;
  private:
    std::filesystem::path sysfsdir_;
  };
}