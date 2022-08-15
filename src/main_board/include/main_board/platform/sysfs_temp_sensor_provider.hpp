#pragma once

#include "device/include/device/temp_sensor.hpp"

namespace mgmt::platform_device {
class TempSensorProvider
{
public:
  constexpr auto compatible() const
  {
    return "sysfs_tempsensor";
  }

  template<class BoardBuilder>
  bool load(BoardBuilder& builder)
  {
    // builder.add_device(mgmt::device::TempSensor_t{});
    throw std::runtime_error("Not implemented");

    return true;
  }
};
}// namespace mgmt::platform_device