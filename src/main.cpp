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

  auto store = hhctrl::core::scheduler::FileTaskStore{"./tasks.json"};


  // auto t = make_zoned(current_zone(), floor<seconds>(system_clock::now()));

  // auto io = boost::asio::io_context{};
  // boost::asio::io_context::work work{io};

  // spdlog::set_level(spdlog::level::debug);

  // auto scheduler = hhctrl::core::scheduler2::Scheduler{io};
  // scheduler.every(std::chrono::seconds(30), []() { spdlog::info("TaskHandler: every 30 seconds task");});
  // scheduler.every(std::chrono::minutes(1), []() { spdlog::info("TaskHandler: every 1 minute task");});
  // scheduler.every(std::chrono::days(1), []() { spdlog::info("TaskHandler: every 1 days task");});
  // scheduler.every(std::chrono::days(2), []() { spdlog::info("TaskHandler: every 1 days task");});

  // io.run();

  // auto sheduler = scheduler::Scheduler{io};

  // sheduler.every("hatch2-isr-open", scheduler::days(1), scheduler::at_time("13:57:15"),
  //   []() { fmt::print("Hatch open\n");}
  // );


  // sheduler.every("hatch2-isr-open-every-seconds", scheduler::seconds(10),
  //   []() { fmt::print("Hatch open\n");}
  // );

  // sheduler.repeat_at("hatch2-isr-close", "13:58:15",
  //   []() { fmt::print("Hatch close\n");}
  // );

  // io.run();

  return 0;
}