#pragma once

#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fmt/format.h>


namespace utils::datetime {
  template<class TSource>
  auto parse(TSource&& src, std::string_view fmt)
  {
    auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto timetm = *std::gmtime(&timet);
    timetm.tm_isdst = -1;
    auto ss = std::istringstream {std::forward<TSource>(src)};
    ss >> std::get_time(&timetm, fmt.data());

    return std::chrono::system_clock::from_time_t(std::mktime(&timetm));
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

  template<class TSource>
  auto from_timestamp(TSource&& since_epoch)
  {
    auto epoch = std::chrono::time_point<std::chrono::high_resolution_clock>();
    return epoch + since_epoch;
  }

  template<class TTimepoint>
  auto get_date(TTimepoint&& tp)
  {
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::forward<TTimepoint>(tp))};
  }

  template<class TTimepoint>
  auto get_time(TTimepoint&& tp)
  {
    const std::time_t tm = std::chrono::system_clock::to_time_t(tp);

    const auto dp = std::chrono::floor<std::chrono::days>(std::forward<TTimepoint>(tp));
    return std::chrono::hh_mm_ss{tp - dp};
  }

  template<class TTimepoint>
  auto to_string_utc(const TTimepoint& tp)
  {
    const auto date = utils::datetime::get_date(tp);
    const auto time = utils::datetime::get_time(tp);

    auto serialize = [](const auto x)
    {
      if (x < 10) {
        return fmt::format("0{}", x);
      } else {
        return fmt::format("{}", x);
      }
    };

    return fmt::format("{}-{}-{} {}:{}:{}",
      serialize(static_cast<unsigned>(date.day())),
      serialize(static_cast<unsigned>(date.month())),
      serialize(static_cast<int>(date.year())),
      serialize(time.hours().count()),
      serialize(time.minutes().count()),
      serialize(time.seconds().count())
    );
  }
}