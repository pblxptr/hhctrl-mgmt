#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include "sysfs_hatch.hpp"
#include "sheduler.hpp"
#include "datetime.hpp"

namespace fs = std::filesystem;

template<class... Ts>
struct overload : Ts...
{
  using Ts::operator()...;
};

int main()
{
  namespace scheduler = hhctrl::core::scheduler;

  auto hatch = hhctrl::hw::SysfsHatch{"~/sysfs/class/hatch2sr"};
  auto sheduler = scheduler::Scheduler{};

  sheduler.add_task("hatch2-isr-open", std::make_unique<scheduler::RepeatedTask>(
    utils::datetime::parse_date("25-03-2021"),
    utils::datetime::parse_date("25-06-2021"),
    utils::datetime::get_time(utils::datetime::parse_time("06:10:35")),
    [&hatch]() { hatch.open(); })
  );

  sheduler.add_task("hatch2-isr-close", std::make_unique<scheduler::RepeatedTask>(
    utils::datetime::parse_date("25-03-2021"),
    utils::datetime::parse_date("25-06-2021"),
    utils::datetime::get_time(utils::datetime::parse_time("22:00:35")),
    [&hatch]() { hatch.close(); })
  );

  // sheduler.use_restore("hatch2-isr", []()
  // {

  // });
}