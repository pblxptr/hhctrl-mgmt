#pragma once

#include <vector>

#include <static/hardware_identity.hpp>
#include <static/device/indicator.hpp>
#include <static/device/temp_sensor.hpp>
#include <variant>
#include <ranges>

namespace mgmt::device
{
  class MainBoard
  {
  public:
    MainBoard(std::vector<Indicator_t> indicators,
      std::vector<TempSensor_t> temp_sensors
    ) : indicators_{std::move(indicators)}
      , temp_sensors_{std::move(temp_sensors)}
    {
      set_indicator_state(IndicatorType::Status, IndicatorState::On);
    }

    std::string hardware_identity() const
    {
      return "HWR1";
    }

    IndicatorState indicator_state(IndicatorType type)
    {
      auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
        return std::visit([](auto&& i) { return i.type(); }, i) == type;
      });

      if (indicator == indicators_.end()) {
        return IndicatorState::NotAvailable;
      }

      return std::visit([](auto&& i) { return i.state(); }, *indicator);
    }

    void set_indicator_state(IndicatorType type, IndicatorState state)
    {
      auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
        return std::visit([](auto&& i) { return i.type(); }, i) == type;
      });

      if (indicator == indicators_.end()) {
        return;
      }

      std::visit([state](auto&& i) { return i.set_state(state); }, *indicator);
    }

    void restart()
    {

    }

  private:
    std::vector<Indicator_t> indicators_;
    std::vector<TempSensor_t> temp_sensors_;
  };
}