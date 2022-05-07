#pragma once

#include <hw/drivers/misc/hatch.hpp>

#include <filesystem>

namespace hw::drivers {
class SysfsHatchDriver : public HatchDriver
{
public:
  explicit SysfsHatchDriver(std::string sysfsdir);
  void open() const override;
  void close() const override;
  HatchStatus status() const override;
private:
  std::filesystem::path sysfsdir_;
};
}