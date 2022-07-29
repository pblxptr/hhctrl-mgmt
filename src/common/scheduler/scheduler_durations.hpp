#pragma once

#include <string>
#include <chrono>

namespace common::scheduler {
using at_time = std::string;
struct days_at
{
  std::chrono::days days;
  at_time at;
};
}// namespace common::scheduler