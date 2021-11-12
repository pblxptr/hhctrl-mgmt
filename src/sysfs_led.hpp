#pragma once

#include "led.hpp"

namespace hhctrl::hw
{
  class SysfsLed : public LedDriver
  {
  public:
    explicit SysfsLed(std::string&);

    void turn_on() const override;
    void turn_off() const override;
  public:
    std::filesystem::path sysfsdir_;
  };
}