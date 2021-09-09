#include "scheduler2.hpp"

#include <spdlog/spdlog.h>

namespace datetime = utils::datetime;

namespace hhctrl::core::scheduler
{
  Scheduler::Scheduler(boost::asio::io_context& io, std::unique_ptr<TaskStore> tasks_store)
    : io_{io}
    , tasks_store_{std::move(tasks_store)}
  {}

  void Scheduler::add_task(std::unique_ptr<Task> new_task)
  {
    if (is_task_active(*new_task)) {
      throw std::runtime_error("Cannot add the task. The same task has been already configured.");
    }
    const bool task_exist = tasks_store_->exist(new_task->id()); //TODO: Consider, whether existing task should be removed, updated or so?
    if (task_exist) {
      const auto existing_task = tasks_store_->find(new_task->id()).value();
      const auto saved_expiry = datetime::from_timestamp(existing_task.timestamp);
      new_task->set_expiry(saved_expiry);
      tasks_store_->remove(new_task->id());
    }

    add_task_to_store(*new_task);
    activate_task(std::move(new_task));
  }

  void Scheduler::add_task_to_store(const Task& task)
  {
    tasks_store_->add(TaskEntity{
    task.id(),
    task.owner(),
    datetime::to_timestamp(task.expiry()) //std::chrono::time_point_cast<std::chrono::milliseconds>(task.expiry()).time_since_epoch().count()
  });
  }

  void Scheduler::activate_task(std::unique_ptr<Task> task)
  {
    task->activate();
    active_tasks_.push_back(std::move(task));
  }

  bool Scheduler::is_task_active(const Task& task) const
  {
    return std::find_if(active_tasks_.begin(), active_tasks_.end(), [&task](const auto& t){
      return task.id() == t->id();
    }) != active_tasks_.end();
  }
}