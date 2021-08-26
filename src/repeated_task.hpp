#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "task.hpp"
#include <spdlog/spdlog.h>

namespace hhctrl::core::scheduler
{
class RepeatedTask : public Task
{
public:
  template<class THandler>
  RepeatedTask(
    boost::asio::io_context& io,
    Id_t id,
    std::string name,
    Timepoint_t expiry,
    THandler&& handler
  )
    : id_{std::move(id)}
    , name_{std::move(name)}
    , timer_{io, std::move(expiry)}
    , handler_{std::forward<THandler>(handler)}
  {}

  const Id_t& id() const
  {
    return id_;
  }

  const std::string& name() const override
  {
    return name_;
  }

  void set_expiry(const Timepoint_t& expiry) override
  {
    timer_.expires_at(expiry);
  }

  void install() override
  {
    spdlog::info(fmt::format("RepeatedTask::install() expiry: {}", utils::datetime::to_string_utc(timer_.expiry())));
    timer_.async_wait([this](const auto& ec)
    {
      if (ec) {
        return;
      }
      handler_();
    });
  }

  void reconfigure() override
  {
    timer_.expires_at(timer_.expiry() + std::chrono::days(1));
  }

  bool is_reinstallable() const override
  {
    return true;
  }

  Timepoint_t expiry() const override
  {
    return timer_.expiry();
  }

  std::string to_string() const override
  {
    return utils::datetime::to_string_utc(timer_.expiry());
  }

private:
  Id_t id_;
  std::string name_;
  TaskHandler_t handler_;
  boost::asio::system_timer timer_;
};
}