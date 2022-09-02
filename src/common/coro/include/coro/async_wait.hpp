#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/steady_timer.hpp>

namespace common::coro {
template<class Duration>
boost::asio::awaitable<void> async_wait(const Duration& duration)
{
  auto executor = co_await boost::asio::this_coro::executor;

  auto timer = boost::asio::steady_timer{ executor };
  timer.expires_after(duration);

  co_await timer.async_wait(boost::asio::use_awaitable);
}
}// namespace common::coro
