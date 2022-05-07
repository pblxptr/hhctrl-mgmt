#pragma once

#include <newnew/device_pooler.hpp>

namespace mgmt::pooler
{
  class HatchPooler : public DevicePooler
  {
  public:
    void pool() override;
  };
}