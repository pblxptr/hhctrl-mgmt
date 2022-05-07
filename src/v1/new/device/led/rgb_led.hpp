#pragma once

#include <mgmt/device/device.hpp>

namespace mgmt::device
{
  class RGBLed  : public Device
  {
  public:
    struct Value
    {
      const uint8_t red;
      const uint8_t green;
      const uint8_t blue;
    };
    explicit RGBLed(DeviceId_t device_id)
      : Device(std::move(device_id))
    {}

    virtual void set_brightness(const Value&) = 0;
  };
}