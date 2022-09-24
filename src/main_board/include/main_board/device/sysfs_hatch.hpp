#pragma once

#include "device/device_id.hpp"
#include "device/hatch.hpp"

#include <filesystem>

namespace mgmt::device {
class SysfsHatch
{
public:
  explicit SysfsHatch(const std::string& sysfsdir);
  // movable
  SysfsHatch(SysfsHatch&&) noexcept = default;
  SysfsHatch& operator=(SysfsHatch&&) noexcept = default;
  // non-copyable
  SysfsHatch(const SysfsHatch&) = delete;
  SysfsHatch& operator=(const SysfsHatch&) = delete;

  ~SysfsHatch() = default;

  void open() const;
  void close() const;
  HatchState status() const;

private:
  std::filesystem::path sysfsdir_;
};
}// namespace mgmt::device
