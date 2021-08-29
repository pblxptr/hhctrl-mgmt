#pragma once

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include "datetime.hpp"
#include <boost/uuid/detail/sha1.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "task_store.hpp"

namespace {
  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::days>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("days_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::hours>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("hours_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::minutes>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("minutes_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::seconds>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("seconds_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::milliseconds>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("milliseconds_{}", std::forward<T>(arg).count());
  }

template<class TDuration> requires
  std::is_same_v<std::decay_t<TDuration>, std::chrono::duration<
    typename std::decay_t<TDuration>::rep,
    typename std::decay_t<TDuration>::period>
  >
  inline std::stringstream& operator<< (std::stringstream& ss, TDuration&& duration)
  {
    ss << duration_to_str(std::forward<TDuration>(duration));
    return ss;
  }

  template<class... TArgs>
  auto generate_id(TArgs&&... args)
  {
    auto gen = boost::uuids::name_generator_sha1{boost::uuids::ns::oid()};
    auto ss = std::stringstream{};

    ((ss << std::forward<TArgs>(args)), ...);

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
  virtual const std::string& owner() const = 0;
  virtual void activate() = 0;
  virtual Timepoint_t expiry() const = 0;
  virtual void set_expiry(Timepoint_t) = 0;
  virtual std::string to_string() const = 0;
};

template<class TDuration, class THandler>
class GenericRepeatedTask : public Task
{
public:
  template<class TDurationArg, class THandlerArg>
  GenericRepeatedTask(
    boost::uuids::uuid id,
    std::string owner,
    boost::asio::io_context& io,
    TDurationArg&& interval,
    THandlerArg&& handler
  )
  : id_{std::move(id)}
  , owner_{std::move(owner)}
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
      timer_.expires_from_now(interval_);
      activate();
    });
  }

  std::string to_string() const override
  {
    using std::to_string;

    return "TaskId: " + to_string(id_) + " expires: " + utils::datetime::to_string(timer_.expiry());
  }

private:
  boost::uuids::uuid id_;
  std::string owner_;
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
    every("anonymous",
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler)
    );
  }

  template<class TOwner, class TDuration, class THandler>
  void every(TOwner&& owner, TDuration&& duration, THandler&& handler)
  {
    add_task(std::make_unique<GenericRepeatedTask<TDuration, THandler>>(
      generate_id(owner, duration),
      std::forward<TOwner>(owner),
      io_,
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler)
    ));
  }

private:
  void add_task(std::unique_ptr<Task>);
  void add_task_to_store(const Task&);
  void activate_task(std::unique_ptr<Task>);
  bool is_task_active(const Task&) const;
private:
  boost::asio::io_context& io_;
  std::unique_ptr<TaskStore> tasks_store_;
  std::vector<std::unique_ptr<Task>> active_tasks_;
};
}