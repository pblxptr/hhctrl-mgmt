#pragma once

#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "datetime.hpp"
#include "scheduler_durations.hpp"
#include "task_store.hpp"
#include "generic_repeated_task.hpp"
#include "everydays_at_task.hpp"

namespace hhctrl::core::scheduler
{
  enum class Execution { Relaxed, Strict };
namespace {
  constexpr auto ANONYMOUS_TASK = "anonymous";

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::milliseconds>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("milliseconds_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::seconds>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("seconds_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::minutes>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("minutes_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::hours>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("hours_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, std::chrono::days>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("days_{}", std::forward<T>(arg).count());
  }

  template<class T> requires std::is_same_v<std::decay_t<T>, days_at>
  inline std::string duration_to_str(T&& arg)
  {
    return fmt::format("days_{}_at_time_{}", arg.days.count(), arg.at);
  }

template<class TDuration> requires
  std::is_same_v<std::decay_t<TDuration>, std::chrono::duration<
    typename std::decay_t<TDuration>::rep,
    typename std::decay_t<TDuration>::period>> ||
  std::is_same_v<std::decay_t<TDuration>, days_at>
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
class Scheduler
{
public:
  Scheduler(boost::asio::io_context&, TaskStore&);

  template<class TDuration, class THandler>
  auto every(TDuration&& duration, THandler&& handler)
  {
    return every(
      ANONYMOUS_TASK,
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler)
    );
  }

  template<class TOwner, class TDuration, class THandler>
  auto every(TOwner&& owner, TDuration&& duration, THandler&& handler)
  {
    return every(
      std::forward<TOwner>(owner),
      Execution{},
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler)
    );
  }

  //Generic repeated task
  template<class TOwner, class TDuration, class THandler>
  auto every(TOwner&& owner, Execution policy, TDuration&& duration, THandler&& handler)
  {
    auto task_id = generate_id(owner, duration);

    add_task(
      std::make_unique<GenericRepeatedTask<TDuration, THandler>>(
        task_id,
        std::forward<TOwner>(owner),
        io_,
        std::forward<TDuration>(duration),
        std::forward<THandler>(handler)),
      policy
    );
    return task_id;
  }

  //EverydayAt task
  template<class TOwner, class TDuration, class THandler> requires std::is_same_v<std::decay_t<TDuration>, days_at>
  auto every(TOwner&& owner, Execution policy, TDuration&& duration, THandler&& handler)
  {
    auto task_id = generate_id(owner, duration);

    add_task(
      std::make_unique<EverydayAtTask<THandler>>(
        generate_id(owner, duration),
        std::forward<TOwner>(owner),
        io_,
        std::forward<TDuration>(duration),
        std::forward<THandler>(handler)),
      policy
    );

    return task_id;
  }

  bool is_task_active(const Task::Id_t& id) const;
  std::vector<TaskInfo> get_active_tasks() const;

private:
  void add_task(std::unique_ptr<Task>, Execution);
  void activate_task(std::unique_ptr<Task>);
  void process_strict_policy_task(Task&);
  void add_task_to_store(const Task&);
private:
  boost::asio::io_context& io_;
  TaskStore& tasks_store_;
  std::vector<std::unique_ptr<Task>> active_tasks_;
};
}