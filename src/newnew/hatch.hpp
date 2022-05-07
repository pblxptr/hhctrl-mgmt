#pragma once

#include <newnew/device.hpp>

namespace mgmt::device
{
  enum class HatchStatus {
    Open,
    Closed,
    ChangingPosition,
    Faulty,
    Undefined
  };
  class Hatch : public GenericDevice<Hatch>
  {
  public:
    explicit Hatch(DeviceId_t device_id)
      : GenericDevice<Hatch>(std::move(device_id))
    {}
    virtual void open() const = 0;
    virtual void close() const = 0;
    virtual HatchStatus status() const = 0;
  };
}