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
    const bool task_exist = tasks_store_->exist(new_task->id());
    if (task_exist) {
      const auto existing_task = tasks_store_->find(new_task->id()).value();
      const auto saved_expiry = datetime::from_timestamp(existing_task.timestamp);
      new_task->set_expiry(saved_expiry);
      tasks_store_->remove(new_task->id());
    }

    tasks_store_->add(TaskEntity{
      new_task->id(),
      "asd",
      std::chrono::time_point_cast<std::chrono::milliseconds>(new_task->expiry()).time_since_epoch().count()
    });
    new_task->install();
    active_tasks_.push_back(std::move(new_task));
  }
}