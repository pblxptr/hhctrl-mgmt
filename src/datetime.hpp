#pragma once

#include <chrono>
#include <sstream>
#include <iomanip>

namespace utils::datetime {
  template<class TSource>
  auto parse(TSource&& src, std::string_view fmt)
  {
    auto tm = std::tm{};
    auto ss = std::stringstream{std::forward<TSource>(src)};
    ss >> std::get_time(&tm, fmt.data());
    const auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    return tp;
  }

  template<class TSource>
  auto parse_date(TSource&& src)
  {
    return parse(std::forward<TSource>(src), "%d-%m-%Y");
  }

  template<class TSource>
  auto parse_time(TSource&& src)
  {
    return parse(std::forward<TSource>(src), "%H:%M:%S");
  }

  template<class TSource>
  auto parse_datetime(TSource&& src)
  {
    return parse(std::forward<TSource>(src), "%d-%m-%Y %H:%M:%S");
  }

  template<class TTimepoint>
  auto get_date(TTimepoint&& tp)
  {
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::forward<TTimepoint>(tp))};
  }

  template<class TTimepoint>
  auto get_time(TTimepoint&& tp)
  {
    const auto dp = std::chrono::floor<std::chrono::days>(tp);
    return std::chrono::hh_mm_ss{floor<std::chrono::milliseconds>(
      std::forward<TTimepoint>(tp) - dp
    )};
  }
}