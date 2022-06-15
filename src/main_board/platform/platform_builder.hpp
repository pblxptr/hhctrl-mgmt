#pragma once

#include <spdlog/spdlog.h>

#include <device/temp_sensor.hpp>
#include <device/indicator.hpp>
#include <main_board/device/main_board.hpp>
#include <main_board/platform/device_loader.hpp>

namespace mgmt::platform_device
{
  template<class GenericDeviceProcessor>
  class PlatformBuilder
  {
  public:
    void add_device(mgmt::device::TempSensor_t temp_sensor)
    {
      spdlog::get("mgmt")->debug("Add device TempSensor_t");

      temp_sensors_.push_back(std::move(temp_sensor));
    }

    void add_device(mgmt::device::Indicator_t indicator)
    {
      spdlog::get("mgmt")->debug("Add device Indicator_t");

      indicators_.push_back(std::move(indicator));
    }

    template<class D>
    void add_loader(DeviceLoader&& loader)
    {
      auto x = [loader = std::move(loader)](const GenericDeviceProcessor& processor) -> bool {
        return processor.template handle<D>(std::move(loader));
      };

      generic_loaders_.push_back(std::move(x));

      spdlog::get("mgmt")->debug("Add generic loader");
    }

    auto build_board() &&
    {
      return mgmt::device::MainBoard{
        std::move(indicators_),
        std::move(temp_sensors_)
      };
    }

    auto build_generic_loaders() &&
    {
      return std::move(generic_loaders_);
    }
  private:
    std::vector<mgmt::device::Indicator_t> indicators_;
    std::vector<mgmt::device::TempSensor_t> temp_sensors_;
    std::vector<std::function<bool(const GenericDeviceProcessor&)>> generic_loaders_;
  };
}