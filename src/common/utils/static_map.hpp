#pragma once

#include <array>
#include <algorithm>
#include <common/logger/logger.hpp>

namespace common::utils {

template<class TKey, class TValue, std::size_t Size>
class StaticMap
{
public:
  using value_type = std::array<std::pair<TKey, TValue>, Size>;

  // Find element by key
  // Throws if not found
  constexpr decltype(auto) at(const TKey& k) const
  {
    auto it = std::find_if(std::begin(data), std::end(data), [&k](const auto& x) { return k == x.first; });

    if (it == data.end()) {
      throw std::range_error(fmt::format("range error, details: {}", common::logger::source()));
    }

    return it->second;
  }

  // Find element by value
  // Throws if not found
  constexpr decltype(auto) at(const TValue& v) const
  {
    auto it = std::find_if(std::begin(data), std::end(data), [&v](const auto& x) { return v == x.second; });

    if (it == data.end()) {
      throw std::range_error(fmt::format("range error, details: {}", common::logger::source()));
    }

    return it->first;
  }

  value_type data;
};
}