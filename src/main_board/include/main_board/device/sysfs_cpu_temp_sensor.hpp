#pragma once

#include <filesystem>

namespace mgmt::device {
class SysfsCPUTempSensor
{
public:
  explicit SysfsCPUTempSensor(std::string);
  float value() const;

private:
  std::filesystem::path sysfsdir_;
};
}// namespace mgmt::device