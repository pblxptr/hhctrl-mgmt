#include <scheduler/scheduler.hpp>

namespace datetime = common::utils::datetime;

namespace common::scheduler {
Scheduler::Scheduler(boost::asio::io_context& ioc, TaskStore& tasks_store)
  : io_{ ioc }
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

bool Scheduler::is_task_active(const Task::Id_t& task_id) const
{
  return std::find_if(active_tasks_.begin(), active_tasks_.end(), [&task_id](const auto& xtask) {
    return task_id == xtask->id();
  }) != active_tasks_.end();
}

std::vector<TaskInfo> Scheduler::get_active_tasks() const
{
  auto tasks = std::vector<TaskInfo>();
  std::transform(active_tasks_.begin(), active_tasks_.end(), std::back_inserter(tasks), [](const auto& xtask) {
    return TaskInfo{
      xtask->id(),
      xtask->owner(),
      xtask->expiry()
    };
  });

  return tasks;
}

void Scheduler::process_strict_policy_task(Task& task)
{
  if (auto existing_task = tasks_store_.find(task.id()); existing_task) {
    const auto saved_expiry = datetime::from_timestamp(existing_task->timestamp);
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
