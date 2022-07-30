#pragma once

#include <vector>
#include <variant>
#include <ranges>

#include <device/hardware_identity.hpp>
#include <device/indicator_t.hpp>
#include <device/temp_sensor.hpp>
#include <device/hardware_identity.hpp>

namespace mgmt::device {
class MainBoard
{
public:
  MainBoard(std::vector<Indicator_t>);
  HardwareIdentity hardware_identity() const;
  IndicatorState indicator_state(IndicatorType type) const;
  void set_indicator_state(IndicatorType type, IndicatorState state);
  void restart();

private:
  std::vector<Indicator_t> indicators_;
};
}// namespace mgmt::device