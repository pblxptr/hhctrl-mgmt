#pragma once

#include <vector>
#include <variant>
#include <ranges>

#include <static/hardware_identity.hpp>
#include <static/device/indicator.hpp>
#include <static/device/temp_sensor.hpp>
#include <static/hardware_identity.hpp>

namespace mgmt::device
{
  class MainBoard
  {
  public:
    MainBoard(std::vector<Indicator_t>, std::vector<TempSensor_t>);
    HardwareIdentity hardware_identity() const;
    IndicatorState indicator_state(IndicatorType type);
    void set_indicator_state(IndicatorType type, IndicatorState state);
    void restart();

  private:
    std::vector<Indicator_t> indicators_;
    std::vector<TempSensor_t> temp_sensors_;
  };
}