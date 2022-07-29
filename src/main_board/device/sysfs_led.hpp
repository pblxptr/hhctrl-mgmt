#pragma once

#include <string>
#include <filesystem>

namespace mgmt::device
{
  class SysfsLed
  {
  public:
    explicit SysfsLed(std::string);
    //movable
    SysfsLed(SysfsLed&&) noexcept = default;
    SysfsLed& operator=(SysfsLed&&) noexcept = default;
    //non-copyable
    SysfsLed(const SysfsLed&) = delete;
    SysfsLed& operator=(const SysfsLed&) = delete;

    int brightness() const;
    void set_brightness(int value);
  private:
  std::filesystem::path sysfsdir_;
  };
}