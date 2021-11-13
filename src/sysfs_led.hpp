#pragma once

#include "led.hpp"

#include <string>
#include <filesystem>

namespace hhctrl::hw
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