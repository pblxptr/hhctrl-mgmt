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
concept Hatch = Device<T> and requires(T obj)
{
  obj.open();
  obj.close();
  {
    obj.status()
    } -> std::same_as<HatchState>;
};
}// namespace mgmt::device
