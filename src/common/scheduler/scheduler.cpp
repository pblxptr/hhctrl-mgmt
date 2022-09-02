#include <scheduler/scheduler.hpp>

#include <spdlog/spdlog.h>

namespace datetime = common::utils::datetime;

namespace common::scheduler {
Scheduler::Scheduler(boost::asio::io_context& io, TaskStore& tasks_store)
  : io_{ io }
  , tasks_store_{ tasks_store }
{}

void Scheduler::add_task(std::unique_ptr<Task> new_task, Execution policy)
{
  if (is_task_active(new_task->id())) {
    throw std::runtime_error("Cannot add the task. The same task has been already configured.");
  }

  if (policy == Execution::Strict) {
    process_strict_policy_task(*new_task);
  }

  activate_task(std::move(new_task));
}

void Scheduler::activate_task(std::unique_ptr<Task> task)
{
  task->activate();
  active_tasks_.push_back(std::move(task));
}

bool Scheduler::is_task_active(const Task::Id_t& id) const
{
  return std::find_if(active_tasks_.begin(), active_tasks_.end(), [&id](const auto& t) {
    return id == t->id();
  }) != active_tasks_.end();
}

std::vector<TaskInfo> Scheduler::get_active_tasks() const
{
  auto tasks = std::vector<TaskInfo>();
  std::transform(active_tasks_.begin(), active_tasks_.end(), std::back_inserter(tasks), [](const auto& t) {
    return TaskInfo{
      t->id(),
      t->owner(),
      t->expiry()
    };
  });

  return tasks;
}

void Scheduler::process_strict_policy_task(Task& task)
{
  if (tasks_store_.exist(task.id())) {
    const auto existing_task = tasks_store_.find(task.id()).value();
    const auto saved_expiry = datetime::from_timestamp(existing_task.timestamp);
    task.set_expiry(saved_expiry);
    tasks_store_.remove(task.id());
  }
  add_task_to_store(task);
}

void Scheduler::add_task_to_store(const Task& task)
{
  tasks_store_.add(TaskEntity{
    task.id(),
    task.owner(),
    datetime::to_timestamp(task.expiry()) });
}
}// namespace common::scheduler