#pragma once

#include <string>
#include <filesystem>

namespace mgmt::device {
class SysfsLed
{
public:
  explicit SysfsLed(const std::string& sysfsdir);
  // movable
  SysfsLed(SysfsLed&&) noexcept = default;
  SysfsLed& operator=(SysfsLed&&) noexcept = default;
  // non-copyable
  SysfsLed(const SysfsLed&) = delete;
  SysfsLed& operator=(const SysfsLed&) = delete;

  ~SysfsLed() = default;

  int brightness() const;
  void set_brightness(int value);

private:
  std::filesystem::path sysfsdir_;
};
}// namespace mgmt::device
