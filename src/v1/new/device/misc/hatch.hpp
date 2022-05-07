#pragma once

#include <optional>
#include <mgmt/device/device.hpp>

namespace mgmt::device {
  enum class HatchStatus2 {
    Open,
    Closed,
    ChangingPosition,
    Faulty,
    Undefined
  };
  class Hatch : public Device
  {
  public:
    explicit Hatch(DeviceId_t device_id)
      : Device(std::move(device_id))
    {}
    virtual void open() const = 0;
    virtual void close() const = 0;
    virtual HatchStatus2 status() const = 0;
  };
}

