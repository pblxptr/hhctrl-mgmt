#pragma once

#include <mgmt/device/device.hpp>

namespace mgmt::device
{
  class Led : public Device
  {
  public:
    explicit Led(DeviceId_t device_id)
      : Device(std::move(device_id))
    {}

    virtual void set_brightness(uint8_t) = 0;
  };
};