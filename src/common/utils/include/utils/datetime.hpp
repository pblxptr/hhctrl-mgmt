#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/format.h>

namespace common::utils::datetime {
constexpr auto DateFmt = std::string_view { "%Y-%m-%d"};
constexpr auto TimeFmt = std::string_view {"%H:%M:%S"};
constexpr auto DatetimeFmt = std::string_view {"%Y-%m-%d %H:%M:%S %Z"};

using Precision_t = std::chrono::milliseconds;

inline auto get_now()
{
  return std::chrono::system_clock::now();
}

template<class T, class TTimepoint>
auto parse_time(T&& time, TTimepoint&& base_timepoint = get_now())
{
  using namespace std::literals;

  auto datetime = fmt::format("{} {}", date::format(DateFmt.data(), std::forward<TTimepoint>(base_timepoint)), std::forward<T>(time));
  auto local = date::local_seconds{};
  auto tmz = std::string{};

  auto input = std::istringstream{ datetime };
  input >> date::parse(DatetimeFmt.data(), local, tmz);

  if (input.fail() || input.bad()) {
    throw std::invalid_argument("Cannot parse datetime due to invalid format.");
  }

  return date::zoned_time(tmz, local).get_sys_time();
}

template<class TSource>
auto from_timestamp(TSource&& since_epoch)
{
  return std::chrono::time_point<std::chrono::system_clock>(Precision_t{since_epoch});
}

template<class TSource>
auto to_timestamp(TSource&& src)
{
  return std::chrono::time_point_cast<Precision_t>(std::forward<TSource>(src)).time_since_epoch().count();
}

template<class TTimepoint>
auto to_string(const TTimepoint& timepoint)
{
  return date::format(DatetimeFmt.data(), timepoint);
}
}// namespace common::utils::datetime
