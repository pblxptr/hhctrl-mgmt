
#pragma once

#include <type_traits>
#include <concepts>

#include <static/device/device.hpp>
#include <static/device/sysfs_temp_sensor.hpp>

namespace mgmt::device {
  template<class T>
  concept TempSensor = Device<T>
  and requires (T t)
  {
    { t.value() } -> const std::same_as<int>;
  };

  using TempSensor_t = SysfsTempSensor;
}