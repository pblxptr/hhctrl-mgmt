#pragma once

#include "hatch.hpp"

#include <string>

namespace hhctrl::hw {
class SysfsHatch
{
public:
  explicit SysfsHatch(std::string sysfsdir);
  void open();
  void close();
  HatchStatus status() const;
private:
  std::string sysfsdir_;
};
}