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
  namespace sch = hhctrl::core::scheduler;

  spdlog::set_level(spdlog::level::debug);

  auto io = boost::asio::io_context{};
  auto work = boost::asio::io_context::work{io};
  auto scheduler = sch::Scheduler{io,
    std::make_unique<sch::FileTaskStore>("./tasks.json")
  };
  scheduler.every(std::chrono::days(2), []() { spdlog::info("TaskHandler: every 1 days task");});
  scheduler.every(sch::days_at(std::chrono::days(1), "20:34:20"), []() { spdlog::info("TaskHandler: everydays at time");});

  io.run();

  return 0;
}