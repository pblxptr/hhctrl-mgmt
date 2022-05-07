#pragma once

#include <newnew/hatch.hpp>

#include <filesystem>

namespace mgmt::device {
class SysfsHatch : public Hatch
{
public:
  explicit SysfsHatch(DeviceId_t, std::string);
  void open() const override;
  void close() const override;
  HatchStatus status() const override;
private:
  std::filesystem::path sysfsdir_;
};
}