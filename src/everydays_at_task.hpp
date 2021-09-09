#pragma once

#include "task.hpp"
#include <boost/asio.hpp>
#include "scheduler_helpers.hpp"

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
    // timer_.expires_from_now(interval_);
  }

  const Id_t& id() const override
  {
    throw std::runtime_error("Not implemented");
  }

  const std::string& owner() const override
  {
    throw std::runtime_error("Not implemented");
  }

  Timepoint_t expiry() const override
  {
    throw std::runtime_error("Not implemented");
  }

  void set_expiry(Timepoint_t tp) override
  {
    throw std::runtime_error("Not implemented");
  }

  void activate() override
  {
    throw std::runtime_error("Not implemented");
  }

  std::string to_string() const override
  {
    throw std::runtime_error("Not implemented");
  }

private:
  boost::uuids::uuid id_;
  std::string owner_;
  boost::asio::system_timer timer_;
  days_at duration_;
  THandler handler_;
  };
}