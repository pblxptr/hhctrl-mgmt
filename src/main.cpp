#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include "sysfs_hatch.hpp"
#include "datetime.hpp"
#include <spdlog/spdlog.h>
#include "date/date.h"
#include "date/tz.h"
#include "scheduler2.hpp"
#include "file_task_store.hpp"

namespace fs = std::filesystem;


template<class... TArgs>
struct overload : public TArgs...
{
  using TArgs::operator()...;
};


template<class...TArgs>
auto make_overload(TArgs&&... args)
{
  return overload<TArgs...>{};
}

int main()
{
  using namespace date;
  using namespace std::chrono;

  spdlog::set_level(spdlog::level::debug);

  auto io = boost::asio::io_context{};
  auto work = boost::asio::io_context::work{io};
  auto scheduler = hhctrl::core::scheduler::Scheduler{io,
    std::make_unique<hhctrl::core::scheduler::FileTaskStore>("./tasks.json")
  };
  // scheduler.every(std::chrono::seconds(30), []() { spdlog::info("TaskHandler: every 30 seconds task");});
  // scheduler.every(std::chrono::minutes(1), []() { spdlog::info("TaskHandler: every 1 minute task");});
  // scheduler.every(std::chrono::days(1), []() { spdlog::info("TaskHandler: every 1 days task");});
  scheduler.every(std::chrono::days(2), []() { spdlog::info("TaskHandler: every 1 days task");});
  scheduler.every(std::chrono::days(2), []() { spdlog::info("TaskHandler: every 1 days task");});

  io.run();

  return 0;
}