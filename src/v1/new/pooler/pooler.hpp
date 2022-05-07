#pragma once

namespace mgmt::pooler
{
public:
  class DevicePooler
  {
  public:
    virtual ~DevicePooler() = default;
    virtual void pool() = 0;
  };
};