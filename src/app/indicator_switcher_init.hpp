#pragma once

#include <boost/asio/steady_timer.hpp>
#include <common/event/event_bus.hpp>
#include <common/coro/co_spawn.hpp>
#include <events/device_created.hpp>
#include <main_board/device/main_board.hpp>
#include <device/device_register.hpp>
#include <spdlog/spdlog.h>

// For purpose of tests //

struct IndicatorSwitcher
{
  mgmt::device::DeviceId_t board_id;

  boost::asio::awaitable<void> run()
  {
    auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(board_id);
    auto coro = co_await boost::asio::this_coro::executor;
    auto timer = boost::asio::steady_timer{ coro };
    auto indicator = mgmt::device::IndicatorType::Status;

    while (true) {
      timer.expires_after(std::chrono::seconds(5));
      co_await timer.async_wait(boost::asio::use_awaitable);

      board.set_indicator_state(indicator, mgmt::device::IndicatorState::On);

      switch (indicator) {
      case mgmt::device::IndicatorType::Status:
        indicator = mgmt::device::IndicatorType::Maintenance;
        break;

      case mgmt::device::IndicatorType::Maintenance:
        indicator = mgmt::device::IndicatorType::Warning;
        break;

      case mgmt::device::IndicatorType::Warning:
        indicator = mgmt::device::IndicatorType::Fault;
        break;

      case mgmt::device::IndicatorType::Fault:
        indicator = mgmt::device::IndicatorType::Status;
        break;
      }
    }
  }
};

namespace mgmt::app {
void indicator_switcher_init(common::event::AsyncEventBus& bus)
{
  bus.subscribe<mgmt::event::DeviceCreated<mgmt::device::MainBoard>>(
    [](auto&& event) -> boost::asio::awaitable<void> {
      spdlog::get("mgmt")->debug("MainBoard device createdasdadsasd, device id: {}", event.device_id);

      auto executor = co_await boost::asio::this_coro::executor;
      auto board_id = event.device_id;

      spdlog::get("mgmt")->debug("Starting switcher");

      boost::asio::co_spawn(
        executor, [board_id]() -> boost::asio::awaitable<void> {
          auto switcher = IndicatorSwitcher{ board_id };
          co_await switcher.run();
        },
        common::coro::rethrow);
    });
}
}// namespace mgmt::app