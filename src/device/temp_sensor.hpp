
#pragma once

#include <type_traits>
#include <concepts>

#include <device/device.hpp>

namespace mgmt::device {
template<class T>
concept TempSensor = Device<T> and requires(T t)
{
  { t.value() } -> const std::same_as<float>;
};

}// namespace mgmt::device