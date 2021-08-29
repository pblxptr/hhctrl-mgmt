#pragma once

#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fmt/format.h>
#include "date/date.h"
#include "date/tz.h"
namespace {
  using namespace std::literals;
}
namespace utils::datetime {
constexpr auto DATE_FMT = "%Y-%m-%d"sv;
constexpr auto TIME_FMT = "%H:%M:%S"sv;
constexpr auto DATETIME_FMT = "%Y-%m-%d %H:%M:%S"sv;

using Precision_t = std::chrono::milliseconds;

template<class T, class TTimepoint>
auto parse_time(T&& time, TTimepoint&& base_timepoint = std::chrono::system_clock::now())
{
  using namespace date;
  using namespace std::literals;

  auto datetime = fmt::format("{} {}", date::format(DATE_FMT.data(), std::forward<TTimepoint>(base_timepoint)), std::forward<T>(time));
  auto utc_tp = std::chrono::system_clock::time_point{};

  auto in = std::istringstream{datetime};
  in >> date::parse(DATETIME_FMT.data(), utc_tp);

  return utc_tp;
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
}