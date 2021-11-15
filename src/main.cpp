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

  spdlog::info("Turning on green led");
  led_service.set_state(hhctrl::hw::IndicatorType::Status, hhctrl::hw::IndicatorState::SteadyOn);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  spdlog::info("Turning on yellow led");
  led_service.set_state(hhctrl::hw::IndicatorType::Warning, hhctrl::hw::IndicatorState::SteadyOn);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  spdlog::info("Turning on blue led");
  led_service.set_state(hhctrl::hw::IndicatorType::Maintenance, hhctrl::hw::IndicatorState::SteadyOn);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  spdlog::info("Turning on red led");
  led_service.set_state(hhctrl::hw::IndicatorType::Fault, hhctrl::hw::IndicatorState::SteadyOn);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  spdlog::info("Turning all led off");
  led_service.set_state(hhctrl::hw::IndicatorType::Fault, hhctrl::hw::IndicatorState::SteadyOff);
  spdlog::info("Led test done.");

  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    led_service.set_state(hhctrl::hw::IndicatorType::Warning, hhctrl::hw::IndicatorState::SteadyOn);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    led_service.set_state(hhctrl::hw::IndicatorType::Warning, hhctrl::hw::IndicatorState::SteadyOff);
  }

  // auto store = scheduler::FileTaskStore("./tasks.json");
  // auto s = scheduler::Scheduler{io, store};
  // s.every(std::chrono::days(1), [](){ spdlog::info("handler: std::chrono::days(1)"); });
  // s.every(std::chrono::days(2), [](){ spdlog::info("handler: std::chrono::days(2)"); });
  // s.every(std::chrono::days(3), [](){ spdlog::info("handler: std::chrono::days(3)"); });

  io.run();

  return 0;
}