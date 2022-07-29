#pragma once

#include <common/executor/executor.hpp>

#include <boost/asio/steady_timer.hpp>

namespace common::timer {
using Timer_t = boost::asio::steady_timer;

Timer_t make_timer(common::executor::Executor_t& executor)
{
  return common::executor::invoke_with_executor(
    executor, [](auto& e) { return Timer_t{ e }; });
}
}// namespace common::timer