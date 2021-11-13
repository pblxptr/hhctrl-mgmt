#pragma once

#include "hatch.hpp"

#include <filesystem>

namespace hhctrl::hw {
class SysfsHatch
{
public:
  explicit SysfsHatch(std::string sysfsdir);
  void open() const;
  void close() const;
  HatchStatus status() const;
private:
  template<class TPath>
  auto get_path(const TPath& path) const { return sysfsdir_ / path; }
private:
  std::filesystem::path sysfsdir_;
};
}