#include "sheduler.hpp"

#include <spdlog/spdlog.h>

namespace {
  bool is_restorable(const hhctrl::core::scheduler::Task& task)
  {
    return true;
  }

  template<class TTimepoint>
  bool can_wait(const hhctrl::core::scheduler::Task& task, const TTimepoint& now)
  {
    return std::chrono::duration_cast<std::chrono::minutes>(task.expiry() - now) > std::chrono::minutes(10)
      && is_restorable(task);
  }
}
namespace hhctrl::core::scheduler
{
Scheduler::Scheduler(boost::asio::io_context& io)
  : io_{io}
  , task_log_{"task_log.txt"}
{}

void Scheduler::add_task(std::unique_ptr<Task> task)
{
  const auto now = std::chrono::system_clock::now();

  spdlog::info("Scheduler::add_task()");
  spdlog::debug(fmt::format("Task info: {}", task->to_string()));

  if (is_task_active(*task)) {
    return;
  }

  if (was_task_executed(*task) && task->is_reinstallable()) {
    const auto existing_task = task_log_.find(task->name());
    task->set_expiry(existing_task.value().timepoint());
    spdlog::debug("Task was already executed. Changing expiry.");
  }

  schedule(std::move(task));
}

void Scheduler::task_completed(const boost::uuids::uuid& id)
{
  const auto task = std::find_if(active_tasks_.begin(), active_tasks_.end(),
    [&id](const auto& t) { return t->id() == id;}
  );

  if ((*task)->is_reinstallable()) {
    (*task)->reconfigure();
    task_log_.erase((*task)->name());
    auto owned_task = std::move(*task);
    active_tasks_.erase(task);
    schedule(std::move(owned_task));
  } else {
    active_tasks_.erase(task);
  }
}
// For new task that are not yet in queue
void Scheduler::schedule(std::unique_ptr<Task> task)
{
  const auto now = std::chrono::system_clock::now();

  add_to_log_if_not_exist(*task);
  task->install();
  active_tasks_.push_back(std::move(task));
}

void Scheduler::add_to_log_if_not_exist(const Task& task)
{
  auto log_entry = task_log_.find(task.name());

  if (!log_entry) {
    task_log_.add(
      TaskLogEntry{task.name(), task.expiry()}
    );
  }
}

bool Scheduler::is_task_active(const Task& task)
{
  return std::find_if(active_tasks_.begin(), active_tasks_.end(),
    [&task](const auto& t) { return t->name() == task.name();}
  ) != active_tasks_.end();
}

bool Scheduler::was_task_executed(const Task& task)
{
  const auto existing_task = task_log_.find(task.name());

  return existing_task.has_value() &&  existing_task.value().timepoint() > task.expiry();
}
}

