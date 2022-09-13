//
// Created by pp on 7/25/22.
//

#pragma once

#include <chrono>
#include <functional>
#include <list>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/awaitable.hpp>
#include <optional>

#include <executor/executor.hpp>
#include <device/device_id.hpp>


namespace mgmt::device {
class PollingService
{
  using Poll_t = std::function<boost::asio::awaitable<void>()>;
  using Timer_t = boost::asio::steady_timer;
  using Interval_t = std::chrono::milliseconds;
  struct Poller
  {
    mgmt::device::DeviceId_t device_id {};
    Interval_t interval {};
    Poll_t poll {};
    std::optional<Timer_t> timer_ {};
  };

public:
  explicit PollingService(common::executor::Executor_t);

  void add_poller(const mgmt::device::DeviceId_t&,
    const Interval_t& interval,
    const Poll_t& poll);

private:
  void start_poller(Poller& poller);

private:
  common::executor::Executor_t executor_;
  std::list<Poller> pollers_;
};
}// namespace mgmt::device
