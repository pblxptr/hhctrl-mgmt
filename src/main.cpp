#include <iostream>
#include <thread>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>

#include "sysfs_hatch.hpp"
#include "datetime.hpp"
#include "scheduler.hpp"
#include "file_task_store.hpp"
#include "rgb_led_service.hpp"
#include "sysfs_led.hpp"

namespace scheduler = hhctrl::core::scheduler;
namespace fs = std::filesystem;

int main()
{

  using namespace date;

  spdlog::set_level(spdlog::level::debug);

  auto io = boost::asio::io_context{};
  auto work = boost::asio::io_context::work{io};

  auto red = hhctrl::hw::SysfsLed{"/sys/class/leds/red"};
  auto green = hhctrl::hw::SysfsLed{"/sys/class/leds/green"};
  auto blue = hhctrl::hw::SysfsLed{"/sys/class/leds/blue"};
  auto led_service = hhctrl::hw::RgbLedService{red, green, blue};

  auto store = scheduler::FileTaskStore("./tasks.json");
  auto s = scheduler::Scheduler{io, store};
  s.every(std::chrono::days(1), [](){ spdlog::info("handler: std::chrono::days(1)"); });
  s.every(std::chrono::days(2), [](){ spdlog::info("handler: std::chrono::days(2)"); });
  s.every(std::chrono::days(3), [](){ spdlog::info("handler: std::chrono::days(3)"); });

  io.run();

  return 0;
}