#pragma once

#include <string>
#include <chrono>

namespace common::scheduler {
using AtTime_t = std::string;
struct DaysAt
{
  std::chrono::days days;
  AtTime_t at;
};
}// namespace common::scheduler
