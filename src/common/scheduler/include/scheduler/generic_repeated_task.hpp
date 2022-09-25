#pragma once

#include <scheduler/task.hpp>
#include <scheduler/logger.hpp>

namespace common::scheduler {
template<class TDuration, class THandler>
class GenericRepeatedTask : public ITask
{
public:
  template<class TDurationArg, class THandlerArg>
  GenericRepeatedTask(
    boost::uuids::uuid task_id,
    std::string owner,
    boost::asio::io_context& ioc,
    TDurationArg&& interval,
    THandlerArg&& handler)
    : id_{ std::move(task_id) }
    , owner_{ std::move(owner) }
    , timer_{ ioc }
    , duration_{ std::forward<TDurationArg>(interval) }
    , handler_{ std::forward<THandlerArg>(handler) }
  {
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

  void set_expiry(Timepoint_t timepoint) override
  {
    common::logger::get(common::scheduler::Logger)->debug(fmt::format("Updating task expiry. From: {}, to: {}", common::utils::datetime::to_string(timer_.expiry()), common::utils::datetime::to_string(timepoint)));
    timer_.expires_at(std::move(timepoint));
  }

  void activate() override
  {
    common::logger::get(common::scheduler::Logger)->debug("Installing task: {}", to_string());

    timer_.async_wait([&](const boost::system::error_code& error_code) {
      if (error_code) {
        spdlog::error("Timer error");
        return;
      }
      handler_();
      configure_expiry();
      activate();
    });
  }

  std::string to_string() const override
  {
    using std::to_string;

    return "GenericRepeatedTask - TaskId: " + to_string(id_) + " expires: " + common::utils::datetime::to_string(timer_.expiry());
  }

private:
  void configure_expiry()
  {
    timer_.expires_from_now(duration_);
  }

private:
  boost::uuids::uuid id_;
  std::string owner_;
  boost::asio::system_timer timer_;
  TDuration duration_;
  THandler handler_;
};
}// namespace common::scheduler
