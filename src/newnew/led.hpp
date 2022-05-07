#pragma once

#include <newnew/device.hpp>

namespace mgmt::device
{
  class Led : public GenericDevice<Led>
  {
  public:
    explicit Led(DeviceId_t device_id)
      : GenericDevice<Led>(std::move(device_id))
    {}

    virtual void set_brightness(uint8_t) = 0;
  };
};