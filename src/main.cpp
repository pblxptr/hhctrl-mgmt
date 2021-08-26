#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include "sysfs_hatch.hpp"
#include "sheduler.hpp"
#include "datetime.hpp"
#include <spdlog/spdlog.h>
#include "date/date.h"
#include "date/tz.h"

namespace fs = std::filesystem;

int main()
{
  using namespace date;
  using namespace std::chrono;

  auto t = make_zoned(current_zone(), floor<seconds>(system_clock::now()));

  // namespace scheduler = hhctrl::core::scheduler;

  // auto io = boost::asio::io_context{};
  // boost::asio::io_context::work work{io};

  // spdlog::set_level(spdlog::level::debug);

  // using namespace std::chrono;


  // auto sheduler = scheduler::Scheduler{io};

  // sheduler.repeat_at("hatch2-isr-open", "13:57:15",
  //   []() { fmt::print("Hatch open\n");}
  // );

  // sheduler.repeat_at("hatch2-isr-close", "13:58:15",
  //   []() { fmt::print("Hatch close\n");}
  // );

  // io.run();
}