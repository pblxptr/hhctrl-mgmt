#pragma once

#include <memory>
#include <new2/pooler/services/device_pooler.hpp>

namespace mgmt::pooler
{
  class PoolingService
  {
  public:
    virtual void add_pooler(std::unique_ptr<DevicePooler> pooler)
    {

    }
  };
}