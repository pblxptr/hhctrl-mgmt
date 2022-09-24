#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utils/datetime.hpp>
#include <scheduler/scheduler_durations.hpp>
#include <scheduler/task_store.hpp>
#include <scheduler/generic_repeated_task.hpp>
#include <scheduler/everydays_at_task.hpp>

namespace common::scheduler {
enum class Execution { Relaxed, Strict };
constexpr auto AnonymousTask = "anonymous";

template<class T>
requires std::is_same_v<std::decay_t<T>, std::chrono::milliseconds>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("milliseconds_{}", std::forward<T>(arg).count());
}

template<class T>
requires std::is_same_v<std::decay_t<T>, std::chrono::seconds>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("seconds_{}", std::forward<T>(arg).count());
}

template<class T>
requires std::is_same_v<std::decay_t<T>, std::chrono::minutes>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("minutes_{}", std::forward<T>(arg).count());
}

template<class T>
requires std::is_same_v<std::decay_t<T>, std::chrono::hours>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("hours_{}", std::forward<T>(arg).count());
}

template<class T>
requires std::is_same_v<std::decay_t<T>, std::chrono::days>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("days_{}", std::forward<T>(arg).count());
}

template<class T>
requires std::is_same_v<std::decay_t<T>, DaysAt>
inline std::string duration_to_str(T&& arg)
{
  return fmt::format("days_{}_AtTime_t_{}", arg.days.count(), arg.at);
}

template<class TDuration>
requires std::is_same_v < std::decay_t<TDuration>, std::chrono::duration < typename std::decay_t<TDuration>::rep,
typename std::decay_t<TDuration>::period >> || std::is_same_v<std::decay_t<TDuration>, DaysAt> inline std::stringstream& operator<<(std::stringstream& stream, TDuration&& duration)
{
  stream << duration_to_str(std::forward<TDuration>(duration));
  return stream;
}

template<class... TArgs>
auto generate_id(TArgs&&... args)
{
  auto gen = boost::uuids::name_generator_sha1{ boost::uuids::ns::oid() };
  auto stream = std::stringstream{};

  ((stream << std::forward<TArgs>(args)), ...);

  return gen(stream.str());
}
class Scheduler
{
public:
  Scheduler(boost::asio::io_context& ioc, ITaskStore& tasks_store);

  template<class TDuration, class THandler>
  auto every(TDuration&& duration, THandler&& handler)
  {
    return every(
      AnonymousTask,
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler));
  }

  template<class TOwner, class TDuration, class THandler>
  auto every(TOwner&& owner, TDuration&& duration, THandler&& handler)
  {
    return every(
      std::forward<TOwner>(owner),
      Execution{},
      std::forward<TDuration>(duration),
      std::forward<THandler>(handler));
  }

  // Generic repeated task
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
      policy);
    return task_id;
  }

  // EverydayAt task
  template<class TOwner, class TDuration, class THandler>
  requires std::is_same_v<std::decay_t<TDuration>, DaysAt>
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
      policy);

    return task_id;
  }

  bool is_task_active(const ITask::Id_t& task_id) const;
  std::vector<TaskInfo> get_active_tasks() const;

private:
  void add_task(std::unique_ptr<ITask> new_task, Execution policy);
  void activate_task(std::unique_ptr<ITask> task);
  void process_strict_policy_task(ITask& task);
  void add_task_to_store(const ITask& task);

private:
  boost::asio::io_context& io_;
  ITaskStore& tasks_store_;
  std::vector<std::unique_ptr<ITask>> active_tasks_;
};
}// namespace common::scheduler
