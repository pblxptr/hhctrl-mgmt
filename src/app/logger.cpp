
#include <app/logger.hpp>
#include <device/logger.hpp>
#include <poller/logger.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <utils/proc.hpp>

namespace {
auto setup_logger(const std::string& logger_name, spdlog::level::level_enum level, std::string_view log_dir)
{
  // Console sink
  auto sinks = std::vector<spdlog::sink_ptr>{};
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(level);
  sinks.push_back(console_sink);

  // Rotate file sink
  if (not log_dir.empty()) {
    // rotate when size exceeds 10mb
    constexpr auto rotate_size = 1024 * 1024 * 10;
    constexpr auto max_files = 3;
    auto rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      fmt::format("{}/{}.txt", log_dir, common::utils::proc::current_name()),
      rotate_size,
      max_files
    );
    sinks.push_back(rotating_file_sink);
  }

  auto logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::trace);
  spdlog::register_logger(logger);
}

} // namespace

namespace mgmt::app
{
  void logger_init(std::string_view log_dir)
  {
    setup_logger("mgmt", spdlog::level::debug, log_dir);
    setup_logger(mgmt::app::Logger, spdlog::level::debug, log_dir);
    setup_logger(mgmt::home_assistant::adapter::Logger, spdlog::level::trace, log_dir);
    setup_logger(mgmt::poller::Logger, spdlog::level::debug, log_dir);
    setup_logger(mgmt::device::Logger, spdlog::level::debug, log_dir);

    setup_logger("async_mqtt_client", spdlog::level::debug, log_dir);
    setup_logger("entity", spdlog::level::debug, log_dir);


    // Override with env variables
    spdlog::cfg::load_env_levels();
  }

} // namespace mgmt::app
