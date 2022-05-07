#pragma once

#include <chrono>

#include <new/device/device.hpp>
#include <new/pooler/pooler.hpp>

namespace mgmt::pooling
{
  using Interval_t = std::chrono::seconds;

  class PoolingSpec
  {
  public:
    std::chrono::seconds interval_;
    mgmt::device::DeviceId_t device_id_;
    std::unique_ptr<DevicePooler> pooler_;
  };

  class PoolingService
  {
  public:
    virtual ~PoolingService() = default;
    virtual void add_pooler(
        std::chrono::seconds,
        const mgmt::device::DeviceId_t&,
        std::unique_ptr<DevicePooler>
      );
    virtual void remove_pooler(const mgmt::device::DeviceId_t&)
  private:
    std::vector<PoolingSpec> specs_;
  };
}