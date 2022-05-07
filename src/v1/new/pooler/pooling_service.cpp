#include <new/pooler/pooler.hpp>

namespace mgmt::pooler
{
  void PoolingService::add_pooler(
    std::chrono::seconds interval,
    const mgmt::device::DeviceId_t& device_id,
    std::unique_ptr<DevicePooler> pooler
  )
  {
    auto it = std::find_if(specs_.begin(), specs_.end(), [&device_id](const auto& s) {
      return s.device_id_ == device_id;
    });

    if (it != specs_.end()) {
      throw std::runtime_error("Pooler for device already exists");
    }

    specs_.push_back(PoolingSpec{
      interval,
      device_id,
      std::move(pooler)
    });
  }

  void PoolingService::remove_pooler(const mgmt::device::DeviceId_t&)
  {
    throw std::runtime_error("Not implemented exception");
  }
}