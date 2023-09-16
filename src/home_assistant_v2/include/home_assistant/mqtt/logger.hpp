//
// Created by bielpa on 26.08.23.
//

#pragma once

#include <spdlog/spdlog.h>

namespace mgmt::home_assistant::mqtt::logger
{
  template <typename... Args>
  void log(std::string_view logger_name, spdlog::level::level_enum level, fmt::format_string<Args...> format, Args&&... args)
  {
    auto logger = spdlog::get(logger_name.data());
    if (!logger) {
      return;
    }

    logger->log(level, format, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void info(std::string_view logger_name, fmt::format_string<Args...> format, Args&&... args)
  {
    log(logger_name, spdlog::level::info, format, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void trace(std::string_view logger_name, fmt::format_string<Args...> format, Args&&... args)
  {
    log(logger_name, spdlog::level::trace, format, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void debug(std::string_view logger_name, fmt::format_string<Args...> format, Args&&... args)
  {
    log(logger_name, spdlog::level::debug, format, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void warn(std::string_view logger_name, fmt::format_string<Args...> format, Args&&... args)
  {
    log(logger_name, spdlog::level::warn, format, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void err(std::string_view logger_name, fmt::format_string<Args...> format, Args&&... args)
  {
    log(logger_name, spdlog::level::err, format, std::forward<Args>(args)...);
  }

  static constexpr auto AsyncMqttClient = "async_mqtt_client";
  static constexpr auto Entity = "entity";
}
