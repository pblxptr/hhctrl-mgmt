#pragma once

#include "task.hpp"
#include <boost/asio.hpp>
#include "datetime.hpp"
#include "date/date.h"
#include "scheduler_durations.hpp"

namespace hhctrl::core::scheduler
{
template<class THandler>
class EverydayAtTask : public Task
{
public:
  template<class TDurationArg, class THandlerArg>
  EverydayAtTask(
    boost::uuids::uuid id,
    std::string owner,
    boost::asio::io_context& io,
    TDurationArg&& duration,
    THandlerArg&& handler
  )
  : id_{std::move(id)}
  , owner_{std::move(owner)}
  , timer_{io}
  , duration_{std::forward<TDurationArg>(duration)}
  , handler_{std::forward<THandlerArg>(handler)}
  {
    //TODO: Add static_assert to check if duration property is constructible from provided argument.

    configure_expiry();
  }

  const Id_t& id() const override
  {
    return id_;
  }

  const std::string& owner() const override
  {
    return owner_;
  }

  Timepoint_t expiry() const override
  {
    return timer_.expiry();
  }

  void set_expiry(Timepoint_t tp) override
  {
    spdlog::debug(fmt::format("Updating task expiry. From: {}, to: {}",
      utils::datetime::to_string(timer_.expiry()),
      utils::datetime::to_string(tp)
    ));
    timer_.expires_at(std::move(tp));
  }

  void activate() override
  {
    spdlog::debug("Installing task: {}", to_string());

    timer_.async_wait([&](const boost::system::error_code& ec) {
      if (ec) {
        spdlog::error("Timer error");
        return;
      }
      handler_();
      timer_.expires_at(timer_.expiry() + duration_.days);
      activate();
    });
  }

  std::string to_string() const override
  {
    using std::to_string;

    return "EverydaysAt - TaskId: " + to_string(id_) + " expires: " + utils::datetime::to_string(timer_.expiry());
  }

private:
  void configure_expiry()
  {
    using namespace date;
    using namespace std::chrono;

    const auto current_tp = utils::datetime::get_now();
    auto expiry_tp = utils::datetime::parse_time(duration_.at, current_tp);

    if (current_tp > expiry_tp) {
      expiry_tp += duration_.days;
    }

    timer_.expires_at(expiry_tp);
  }

private:
  boost::uuids::uuid id_;
  std::string owner_;
  boost::asio::system_timer timer_;
  days_at duration_;
  THandler handler_;
  };
}