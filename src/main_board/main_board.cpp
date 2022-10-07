#include <main_board/device/main_board.hpp>

#include <ranges>
#include <algorithm>
#include <unistd.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include "device/logger.hpp"

namespace mgmt::device {
MainBoard::MainBoard(std::vector<Indicator_t> indicators)
  : indicators_{ std::move(indicators) }
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  set_indicator_state(IndicatorType::Maintenance, IndicatorState::Off);
  set_indicator_state(IndicatorType::Maintenance, IndicatorState::On);
}

HardwareIdentity MainBoard::hardware_identity() const// NOLINT(readability-convert-member-functions-to-static)
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

  auto indicator = std::ranges::find_if(indicators_, [type](auto&& xindicator) {
    return xindicator.type() == type;
  });

  if (indicator == indicators_.end()) {
    return IndicatorState::NotAvailable;
  }

  return indicator->state();
}

void MainBoard::set_indicator_state(IndicatorType type, IndicatorState state)
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  // TODO(pp): Add concepts to constraint auto&&
  auto indicator = std::ranges::find_if(indicators_, [type](auto&& xindicator) {
    return xindicator.type() == type;
  });

  if (indicator == indicators_.end()) {
    return;
  }

  indicator->set_state(state);
}

void MainBoard::restart()// NOLINT(readability-convert-member-functions-to-static)
{
  common::logger::get(mgmt::device::Logger)->debug("MainBoard::{}", __FUNCTION__);

  sync();

  if (reboot(LINUX_REBOOT_CMD_RESTART) < 0) { /* It should never happen */
    common::logger::get(mgmt::device::Logger)->error("Restart trigger has failed. Reset the board");
  }

}
}// namespace mgmt::device
