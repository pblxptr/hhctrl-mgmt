#pragma once

#include <array>
#include <algorithm>
#include "logger/logger.hpp"

namespace common::utils {

template<class TKey, class TValue, std::size_t size>
class StaticMap
{
public:
  using ValueType_t = std::array<std::pair<TKey, TValue>, size>;


  constexpr decltype(auto) at(std::size_t index) const
  {
    if (index > size - 1) {
      throw std::out_of_range{"Index is out of range"};
    }

    return data[index].second;
  }

  // Find element by key
  // Throws if not found
  constexpr decltype(auto) at(const TKey& key) const
  {
    auto elem = std::find_if(std::begin(data), std::end(data), [&key](const auto& xelem) { return key == xelem.first; });

    if (elem == data.end()) {
      throw std::range_error(fmt::format("range error, details: {}", common::logger::source()));
    }

    return elem->second;
  }

  // Find element by value
  // Throws if not found
  constexpr decltype(auto) at(const TValue& value) const
  {
    auto elem = std::find_if(std::begin(data), std::end(data), [&value](const auto& xelem) { return value == xelem.second; });

    if (elem == data.end()) {
      throw std::range_error(fmt::format("range error, details: {}", common::logger::source()));
    }

    return elem->first;
  }

  constexpr bool contains(const TKey& key) const
  {
    auto elem = std::find_if(std::begin(data), std::end(data), [&key](const auto& xelem) { return key == xelem.first; });

    return elem != data.end();
  }

  ValueType_t data;
};
}// namespace common::utils
