#include <newnew/device_pooling_manager.hpp>
#include <ranges>
#include <algorithm>
#include <fmt/format.h>
#include <string>

namespace mgmt::pooler
{
  void DevicePoolingManager::add_pooler(
      const mgmt::device::DeviceId& device_id,
      Interval_t interval,
      std::unique_ptr<DevicePooler> pooler
    )
  {
    using std::to_string;

    if (std::ranges::any_of(configs_, [&device_id](const auto& cfg) {
      return cfg.device_id == device_id;
    })) {
      throw std::runtime_error(fmt::format("Pooler for id: {} already exists", to_string(device_id)));
    }

    configs_.emplace_back(device_id, interval, std::move(pooler));
  }
}