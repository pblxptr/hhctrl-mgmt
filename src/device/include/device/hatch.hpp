#pragma once

#include <concepts>
#include <string>

#include "device.hpp"

namespace mgmt::device {
enum class HatchState { Undefined,
  Open,
  Closed,
  ChangingPosition,
  Faulty };

inline std::string to_string(const HatchState& state)
{
  using enum HatchState;

  switch (state) {
  case Undefined:
    return "Undefined";
  case Open:
    return "Open";
  case Closed:
    return "Closed";
  case ChangingPosition:
    return "ChangingPosition";
  case Faulty:
    return "Faulty";
  }
  return "";
}

template<class T>
concept Hatch = Device<T> and requires(T obj)
{
  obj.open();
  obj.close();
  {
    obj.status()
    } -> std::same_as<HatchState>;
};
}// namespace mgmt::device
