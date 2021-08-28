#include "scheduler2.hpp"

#include <spdlog/spdlog.h>

namespace hhctrl::core::scheduler
{
  Scheduler::Scheduler(boost::asio::io_context& io, std::unique_ptr<TaskStore> tasks_store)
    : io_{io}
    , tasks_store_{std::move(tasks_store)}
  {}

  void Scheduler::add_task(std::unique_ptr<Task> task)
  {
    task->install();
    tasks_store_->add(TaskEntity{
      task->id(),
      "asd",
      task->expiry().time_since_epoch().count()
    });
    active_tasks_.push_back(std::move(task));
  }
}