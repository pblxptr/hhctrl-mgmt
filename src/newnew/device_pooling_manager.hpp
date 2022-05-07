#pragma once

#include <newnew/device_pooler.hpp>
#include <newnew/device_id.hpp>
#include <chrono>
#include <memory>
#include <vector>

namespace mgmt::pooler
{
  class DevicePoolingManager
  {
    using Interval_t = std::chrono::seconds;

    struct PoolingConfig
    {
      mgmt::device::DeviceId device_id;
      Interval_t interval;
      std::unique_ptr<DevicePooler> pooler;
    };
  public:
    virtual ~DevicePoolingManager() = default;

    void add_pooler(const mgmt::device::DeviceId&, Interval_t, std::unique_ptr<DevicePooler>);
  private:
    std::vector<PoolingConfig> configs_;
  };
}