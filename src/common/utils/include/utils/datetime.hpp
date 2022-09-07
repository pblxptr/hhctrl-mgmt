#pragma once

#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/format.h>

namespace {
using namespace std::literals;
namespace dt = date;
}// namespace
namespace common::utils::datetime {
constexpr auto DATE_FMT = "%Y-%m-%d"sv;
constexpr auto TIME_FMT = "%H:%M:%S"sv;
constexpr auto DATETIME_FMT = "%Y-%m-%d %H:%M:%S %Z"sv;

using Precision_t = std::chrono::milliseconds;

inline auto get_now()
{
  return std::chrono::system_clock::now();
}

template<class T, class TTimepoint>
auto parse_time(T&& time, TTimepoint&& base_timepoint = get_now())
{
  using namespace std::literals;

  auto datetime = fmt::format("{} {}", date::format(DATE_FMT.data(), std::forward<TTimepoint>(base_timepoint)), std::forward<T>(time));
  auto local = dt::local_seconds{};
  auto tmz = std::string{};

  auto in = std::istringstream{ datetime };
  in >> dt::parse(DATETIME_FMT.data(), local, tmz);

  if (in.fail() || in.bad()) {
    throw std::invalid_argument("Cannot parse datetime due to invalid format.");
  }

  return dt::zoned_time(tmz, local).get_sys_time();
}

template<class TSource>
auto from_timestamp(TSource&& since_epoch)
{
  return std::chrono::time_point<std::chrono::system_clock>(Precision_t(since_epoch));
}

template<class TSource>
auto to_timestamp(TSource&& src)
{
  return std::chrono::time_point_cast<Precision_t>(std::forward<TSource>(src)).time_since_epoch().count();
}

template<class TTimepoint>
auto to_string(const TTimepoint& tp)
{
  return date::format(DATETIME_FMT.data(), tp);
}
}// namespace common::utils::datetime
