#pragma once

#include <concepts>
#include "device.hpp"
namespace mgmt::device {
enum class HatchState { Undefined,
  Open,
  Closed,
  ChangingPosition,
  Faulty };

template<class T>
concept Hatch = Device<T> and requires(T v)
{
  v.open();
  v.close();
  {
    v.status()
    } -> std::same_as<HatchState>;
};
}// namespace mgmt::device