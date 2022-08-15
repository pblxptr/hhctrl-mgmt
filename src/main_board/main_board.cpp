#include <main_board/device/main_board.hpp>

#include <ranges>
#include <algorithm>

#include "device/logger.hpp"

namespace mgmt::device {
MainBoard::MainBoard(std::vector<Indicator_t> indicators)
  : indicators_{ std::move(indicators) }
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  set_indicator_state(IndicatorType::Maintenance, IndicatorState::Off);
  set_indicator_state(IndicatorType::Maintenance, IndicatorState::On);
}

HardwareIdentity MainBoard::hardware_identity() const
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  return HardwareIdentity{
    .manufacturer = "BHome",
    .model = "HenhouseCtrl",
    .revision = "R1",
    .serial_number = "s000010123D"
  };
}

IndicatorState MainBoard::indicator_state(IndicatorType type) const
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
    return i.type() == type;
  });

  if (indicator == indicators_.end()) {
    return IndicatorState::NotAvailable;
  }

  return indicator->state();
}

void MainBoard::set_indicator_state(IndicatorType type, IndicatorState state)
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  // TODO: Add concepts to constraint auto&&
  auto indicator = std::ranges::find_if(indicators_, [type](auto&& i) {
    return i.type() == type;
  });

  if (indicator == indicators_.end()) {
    return;
  }

  indicator->set_state(state);
}

void MainBoard::restart()
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);
}
}// namespace mgmt::device