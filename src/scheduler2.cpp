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

    active_tasks_.push_back(std::move(task));
  }
}