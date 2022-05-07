#pragma once

#include <new/device/misc/hatch.hpp>

#include <filesystem>

namespace mgmt::device {
class SysfsHatch : public Hatch
{
public:
  explicit SysfsHatch(DeviceId_t, std::string);
  void open() const override;
  void close() const override;
  HatchStatus2 status() const override;
private:
  std::filesystem::path sysfsdir_;
};
}