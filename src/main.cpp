#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "sysfs_hatch.hpp"
#include "datetime.hpp"
#include "scheduler.hpp"
#include "file_task_store.hpp"

namespace fs = std::filesystem;

int main()
{
  namespace scheduler = hhctrl::core::scheduler;

  spdlog::set_level(spdlog::level::debug);

  auto io = boost::asio::io_context{};
  auto work = boost::asio::io_context::work{io};
  auto store = scheduler::FileTaskStore("./tasks.json");
  auto s = scheduler::Scheduler{io, store};

  s.every(scheduler::days_at(std::chrono::days(1), "17:34:20 Europe/Warsaw"), []() { spdlog::info("TaskHandler: everydays at time");});

  io.run();

  return 0;
}