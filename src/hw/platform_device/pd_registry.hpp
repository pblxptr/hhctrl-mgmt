#pragma once

#include <hw/drivers/driver.hpp>

#include <vector>

namespace hw::platform_device
{
  class PlatformDeviceRegistry
  {
  public:
    void add_to_registry(std::unique_ptr<hw::drivers::Driver> driver)
    {
      drivers_.push_back(std::move(driver));
    }

    hw::drivers::Driver* get_driver() const
    {
      return nullptr;
    }

  private:
    std::vector<hw::drivers::Driver> drivers_;
  };
}