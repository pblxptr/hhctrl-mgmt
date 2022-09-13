//
// Created by pp on 7/25/22.
//
#include <algorithm>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <poller/polling_service.hpp>
#include <coro/co_spawn.hpp>
#include <device/logger.hpp>

namespace mgmt::device {
PollingService::PollingService(common::executor::Executor_t executor)
  : executor_{ std::move(executor) }
{}

void PollingService::add_poller(
  const mgmt::device::DeviceId_t& device_id,
  const Interval_t& interval,
  const Poll_t& poll)
{
  if (std::ranges::count(pollers_, device_id, &Poller::device_id) != 0) {
    throw std::runtime_error("Poller for requested device id, already exists");
  }

  pollers_.push_back(Poller{ device_id, interval, poll });
  start_poller(pollers_.back());
}

void PollingService::start_poller(Poller& poller)
{
  common::executor::invoke(executor_, [&poller](auto&& e) {
    auto poll = [&poller, &e]() -> boost::asio::awaitable<void> {
      auto& timer = poller.timer_;
      timer.emplace(Timer_t{ e });

      for (;;) {
        auto error_code = boost::system::error_code{};
        timer->expires_from_now(poller.interval, error_code);
        co_await timer->async_wait(boost::asio::use_awaitable);

        if (error_code) {
          common::logger::get(mgmt::device::Logger)->debug("Poller error_code: {}", error_code.message());
          timer.reset();
          break;
        }
        common::logger::get(mgmt::device::Logger)->debug("Poll device with id: {}", poller.device_id);
        co_await poller.poll();
      };
    };
    boost::asio::co_spawn(e, std::move(poll), common::coro::rethrow);
  });
}

}// namespace mgmt::device
