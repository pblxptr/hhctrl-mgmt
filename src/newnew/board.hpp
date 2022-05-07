#pragma once

#include <newnew/device.hpp>
#include <newnew/hardware_identity.hpp>

namespace mgmt::device
{
  class Board : public GenericDevice<Board>
  {
  public:
    explicit Board(DeviceId_t id) : GenericDevice<Board>{std::move(id)}
      {}
    virtual void restart() = 0;
    virtual HardwareIdentity hardware_identity() = 0;
  };
}
