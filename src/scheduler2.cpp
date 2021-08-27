#include "scheduler2.hpp"

#include <spdlog/spdlog.h>

namespace hhctrl::core::scheduler2
{
  Scheduler::Scheduler(boost::asio::io_context& io)
    : io_{io}
  {}

  void Scheduler::add_task(std::unique_ptr<Task> task)
  {
    task->install();

    active_tasks_.push_back(std::move(task));
  }
}