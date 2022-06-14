#include <main_board/main_board.hpp>

#include <ranges>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace mgmt::device
{
  MainBoard::MainBoard(
    std::vector<Indicator_t> indicators,
    std::vector<TempSensor_t> temp_sensors
  ) : indicators_{std::move(indicators)}
    , temp_sensors_{std::move(temp_sensors)}
  {
    set_indicator_state(IndicatorType::Maintenance, IndicatorState::Off);
    set_indicator_state(IndicatorType::Maintenance, IndicatorState::On);
  }

  HardwareIdentity MainBoard::hardware_identity() const
  {
    spdlog::get("mgmt")->debug("{}", __FUNCTION__);

    return HardwareIdentity {
      .model = "HenhouseCtrl",
      .revision = "R1",
      .serial_number = "serial_number"
    };
  }

  IndicatorState MainBoard::indicator_state(IndicatorType type)
  {
    spdlog::get("mgmt")->debug("{}", __FUNCTION__);

    auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
      return std::visit([](auto&& i) { return i.type(); }, i) == type;
    });

    if (indicator == indicators_.end()) {
      return IndicatorState::NotAvailable;
    }

    return std::visit([](auto&& i) { return i.state(); }, *indicator);
  }

  void MainBoard::set_indicator_state(IndicatorType type, IndicatorState state)
  {
    spdlog::get("mgmt")->debug("{}", __FUNCTION__);

    //TODO: Add concepts to constraint auto&&
    auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
      return std::visit([](auto&& i) { return i.type(); }, i) == type;
    });

    if (indicator == indicators_.end()) {
      return;
    }

    std::visit([state](auto&& i) { return i.set_state(state); }, *indicator);
  }

  void MainBoard::restart()
  {
    spdlog::get("mgmt")->debug("{}", __FUNCTION__);
  }
}