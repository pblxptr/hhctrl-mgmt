#pragma once

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include "datetime.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "task_store.hpp"

namespace {
  inline std::string duration_to_str(const std::chrono::days& days)
  {
    return fmt::format("days_{}", days.count());
  }

  inline std::string duration_to_str(const std::chrono::hours& hours)
  {
    return fmt::format("hours_{}", hours.count());
  }

  inline std::string duration_to_str(const std::chrono::minutes& minutes)
  {
    return fmt::format("minutes_{}", minutes.count());
  }

  inline std::string duration_to_str(const std::chrono::seconds& seconds)
  {
    return fmt::format("seconds_{}", seconds.count());
  }

  inline std::string duration_to_str(const std::chrono::milliseconds& milliseconds)
  {
    return fmt::format("milliseconds_{}", milliseconds.count());
  }

  template<class TDuration>
  auto generate_id(const std::string& str, const TDuration& duration)
  {
    auto gen = boost::uuids::name_generator_sha1{boost::uuids::ns::oid()};
    auto ss = std::stringstream{};
    ss << str << duration_to_str(duration);

    return gen(ss.str());
  }
}
namespace hhctrl::core::scheduler
{
class Task
{
public:
  using Id_t = boost::uuids::uuid;
  using Timepoint_t = std::chrono::time_point<std::chrono::system_clock>;

  ~Task() = default;
  virtual const Id_t& id() const = 0;
  virtual void install() = 0;
  virtual Timepoint_t expiry() const = 0;
  virtual std::string to_string() const = 0;
};

template<class TDuration, class THandler>
class GenericRepeatedTask : public Task
{
public:
  template<class TDurationArg, class THandlerArg>
  GenericRepeatedTask(
    boost::uuids::uuid id,
    boost::asio::io_context& io,
    TDurationArg&& interval,
    THandlerArg&& handler
  )
  : id_{std::move(id)}
  , timer_{io}
  , interval_{std::forward<TDurationArg>(interval)}
  , handler_{std::forward<THandlerArg>(handler)}
  {
    timer_.expires_from_now(interval_);
  }

  const Id_t& id() const override
  {
    return id_;
  }

  Timepoint_t expiry() const override
  {
    return timer_.expiry();
  }

  void install() override
  {
    spdlog::debug("Installing task: {}", to_string());

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

  std::string to_string() const override
  {
    using std::to_string;

    return "TaskId: " + to_string(id_) + " expires: " + utils::datetime::to_string(timer_.expiry());
  }

private:
  boost::uuids::uuid id_;
  boost::asio::system_timer timer_;
  TDuration interval_;
  THandler handler_;
};

class Scheduler
{
public:
  explicit Scheduler(boost::asio::io_context&, std::unique_ptr<TaskStore>);

  template<class TDuration, class THandler>
  void every(TDuration&& duration, THandler&& handler)
  {
    add_task(std::make_unique<GenericRepeatedTask<TDuration, THandler>>(
      generate_id("anonymous", duration),
      io_,
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler)
    ));
  }

private:
  void add_task(std::unique_ptr<Task>);
private:
  boost::asio::io_context& io_;
  std::unique_ptr<TaskStore> tasks_store_;
  std::vector<std::unique_ptr<Task>> active_tasks_;
};
}