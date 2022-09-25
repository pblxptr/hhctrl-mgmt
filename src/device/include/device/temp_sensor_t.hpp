
#pragma once

#include <type_traits>
#include <concepts>
#include <variant>

#include <device/temp_sensor.hpp>
#include <main_board/device/sysfs_ds18b20.hpp>
#include <main_board/device/sysfs_cpu_temp_sensor.hpp>

namespace mgmt::device {

template<TempSensor... SupportedTypes>
class TempSensorProxy
{
  using TempSensorVariant_t = std::variant<SupportedTypes...>;

public:
  explicit TempSensorProxy(TempSensorVariant_t temp_sensor)
    : temp_sensor_{ std::move(temp_sensor) }
  {}

  // movable
  TempSensorProxy(TempSensorProxy&&) noexcept = default;
  TempSensorProxy& operator=(TempSensorProxy&&) noexcept = default;
  // non-copyable
  TempSensorProxy(const TempSensorProxy&) = delete;
  TempSensorProxy& operator=(const TempSensorProxy&) = delete;

  ~TempSensorProxy() = default;

  auto value() const
  {
    return std::visit([](auto&& sensor) { return sensor.value(); }, temp_sensor_);
  }

private:
  TempSensorVariant_t temp_sensor_;
};

using TempSensor_t = TempSensorProxy<SysfsDS18B20, SysfsCPUTempSensor>;
}// namespace mgmt::device
