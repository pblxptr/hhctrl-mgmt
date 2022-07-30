
#pragma once

#include <type_traits>
#include <concepts>

#include <device/temp_sensor.hpp>
#include <main_board/device/sysfs_ds18b20.hpp>

namespace mgmt::device {

template<TempSensor... SupportedTypes>
class TempSensorProxy
{
  using TempSensorVariant_t = std::variant<SupportedTypes...>;

public:
  explicit TempSensorProxy(TempSensorVariant_t temp_sensor)
    : temp_sensor_{ std::move(temp_sensor) }
  {}

  TempSensorProxy(const TempSensorProxy&) = delete;
  TempSensorProxy& operator=(const TempSensorProxy&) = delete;
  TempSensorProxy(TempSensorProxy&&) noexcept = default;
  TempSensorProxy& operator=(TempSensorProxy&&) noexcept = default;

  auto value() const
  {
    return std::visit([](auto&& v) { return v.value(); }, temp_sensor_);
  }

private:
  TempSensorVariant_t temp_sensor_;
};

using TempSensor_t = TempSensorProxy<
  SysfsDS18B20>;
}// namespace mgmt::device