#pragma once

#include <chrono>
#include <functional>
#include <memory>

namespace hhctrl::core::scheduler
{
template<class... Ts>
struct overload : Ts...
{
  using Ts::operator()...;
};

using Timepoint_t = std::chrono::time_point<std::chrono::system_clock>;
using TaskHandler_t = std::function<void()>;

class Task
{
public:
  virtual ~Task() = default;
  virtual void activate() = 0;
  virtual void execute() = 0;
  virtual void serialize() = 0;
};

class RepeatedTask : public Task
{
public:
  template<class T, class... TArgs>
  RepeatedTask(
    Timepoint_t from,
    Timepoint_t to,
    std::chrono::hh_mm_ss<T> triggers_at,
    TArgs&&... args
  )
    : handler_{std::forward<TArgs...>(args...)}
  {}

  void activate() override
  {

  }

  void execute() override
  {
  }

  void serialize() override
  {
  }

private:
  TaskHandler_t handler_;
};

struct RepeatedTaskRestore {};
struct OneShotTaskRestore{};

struct Restore
{
  virtual ~Restore() = default;
  virtual void restore(const RepeatedTaskRestore&) = 0;
  virtual void restore(const OneShotTaskRestore&) = 0;
};

template<class T>
struct GenericRestore : Restore
{
  template<class... TArgs>
  GenericRestore(TArgs&&... args)
    : handler{std::forward<TArgs...>(args...)}
  {}

  void restore(const RepeatedTaskRestore& arg) override
  {
    handler(arg);
  }
  void restore(const OneShotTaskRestore& arg) override
  {
    handler(arg);
  }

  T handler;
};

class Scheduler
{
private:
  using Restore_t = std::pair<std::string, std::unique_ptr<Restore>>;
public:
  Scheduler();
  void add_task(std::string id, std::unique_ptr<Task> action);

  template<class... T>
  auto use_restore(std::string id, T&&...)
  {
    using RestoreHandler_t = overload<T...>;

    add_restore(std::pair(std::move(id), std::make_unique<GenericRestore<RestoreHandler_t>>(RestoreHandler_t{})));
  }
private:
  void add_restore(Restore_t restore);
};
}