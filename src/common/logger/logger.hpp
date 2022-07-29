#pragma once

#include <string_view>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <source_location>

namespace common::logger {
inline auto source(const std::source_location location = std::source_location::current())
{
  return fmt::format("file name: {}({}:{})\nfunc name: {}",
    location.file_name(),
    location.line(),
    location.column(),
    location.function_name());
}

inline auto get(const std::string& logger_name)
{
  auto log = spdlog::get(logger_name);

  if (log) {
    return log;
  } else {
    auto null_logger = spdlog::get("null_logger");

    if (not null_logger) {
      null_logger = spdlog::create<spdlog::sinks::null_sink_mt>("null_logger");
    }

    return null_logger;
  }
}
}// namespace common::logger