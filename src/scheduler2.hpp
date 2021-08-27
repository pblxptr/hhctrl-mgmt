#pragma once

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include "datetime.hpp"

namespace hhctrl::core::scheduler2
{

class Task
{
public:
  ~Task() = default;
  virtual void install() = 0;
  virtual std::string to_string() const = 0;
};

template<class TInterval, class THandler>
class GenericRepeatedTask : public Task
{
public:
  template<class TIntervalArg, class THandlerArg>
  GenericRepeatedTask(
    boost::asio::io_context& io,
    TIntervalArg&& interval,
    THandlerArg&& handler
  )
  : timer_{io}
  , interval_{std::forward<TIntervalArg>(interval)}
  , handler_{std::forward<THandlerArg>(handler)}
  {
    timer_.expires_from_now(interval_);
  }

  void install()
  {
    spdlog::debug("Installing task - expires at: {}", to_string());

    timer_.async_wait([&](const boost::system::error_code& ec) {
      if (ec) {
        spdlog::error("Timer error");
        return;
      }
      handler_();
      timer_.expires_from_now(interval_);
      install();
    });
  }

  std::string to_string() const
  {
    return utils::datetime::to_string(timer_.expiry());
  }
private:
  boost::asio::system_timer timer_;
  TInterval interval_;
  THandler handler_;
};


class Scheduler
{
public:
  explicit Scheduler(boost::asio::io_context& io);

  template<class TInterval, class THandler>
  void every(TInterval&& interval, THandler&& handler)
  {
    add_task(std::make_unique<GenericRepeatedTask<TInterval, THandler>>(
      io_,
      std::forward<TInterval>(interval),
      std::forward<THandler>(handler)
    ));
  }
  template<class THandler>
  void every_at(const std::chrono::days& days, const std::string& at, THandler&& handler)
  {

  }
private:
  void add_task(std::unique_ptr<Task>);
private:
  boost::asio::io_context& io_;
  std::vector<std::unique_ptr<Task>> active_tasks_;
};
}