#include <main_board/device/main_board.hpp>

#include <ranges>
#include <algorithm>
#include <unistd.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <fstream>
#include <device/logger.hpp>

namespace {
constexpr auto HwNamePath = "/etc/hw-name";
constexpr auto HwSerialPath = "/etc/hw-serial";

std::string get_hw_info_attr(const std::string_view path)
{
  auto file = std::ifstream { path.data() };
  if (!file) {
    common::logger::get(mgmt::device::Logger)->warn("MainBoard::{}, missing hw attr with path: {}", __FUNCTION__, path);

    return {fmt::format("default-{}", path)};
  }

  auto value = std::string{};

  while (!file.eof()) {
    const char val = static_cast<char>(file.get());
    if (val == '\n' || val == '\0') {
      break;
    }
    value += val;
  }

  return value;
}
} // namespace

namespace mgmt::device {
MainBoard::MainBoard(std::vector<Indicator_t> indicators)
  : indicators_{ std::move(indicators) }
{
  common::logger::get(mgmt::device::Logger)->trace("MainBoard::{}", __FUNCTION__);

  set_indicator_state(IndicatorType::Status, IndicatorState::Off);
  set_indicator_state(IndicatorType::Status, IndicatorState::On);
}

HardwareIdentity MainBoard::hardware_identity() const// NOLINT(readability-convert-member-functions-to-static)
{
  common::logger::get(mgmt::device::Logger)->trace("MainBoard::{}", __FUNCTION__);

  return HardwareIdentity{
    .manufacturer = "BHome",
    .model = get_hw_info_attr(HwNamePath),
    .revision = "R1",
    .serial_number = get_hw_info_attr(HwSerialPath)
  };
}

IndicatorState MainBoard::indicator_state(IndicatorType type) const
{
  common::logger::get(mgmt::device::Logger)->trace("MainBoard::{}", __FUNCTION__);

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
  common::logger::get(mgmt::device::Logger)->trace("MainBoard::{}", __FUNCTION__);

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
  common::logger::get(mgmt::device::Logger)->trace("MainBoard::{}", __FUNCTION__);

  sync();

  if (reboot(LINUX_REBOOT_CMD_RESTART) < 0) { /* It should never happen */
    common::logger::get(mgmt::device::Logger)->error("Restart trigger has failed. Reset the board");
  }

}
}// namespace mgmt::device

