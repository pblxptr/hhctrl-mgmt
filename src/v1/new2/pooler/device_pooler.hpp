#pragma once

namespace mgmt::pooler
{
  class DevicePooler
  {
  public:
    virtual ~DevicePooler() = default;
    virtual void poll() = 0;
  };
}