#pragma once

#include <filesystem>

namespace mgmt::device {
class SysfsDS18B20
{
public:
  explicit SysfsDS18B20(std::string);
  float value() const;
private:
  std::filesystem::path sysfsdir_;
};
}// namespace mgmt::device