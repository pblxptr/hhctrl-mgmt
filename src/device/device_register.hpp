#pragma once

#include <atomic>
#include <spdlog/spdlog.h>

#include <device/inventory.hpp>
#include <device/device_id.hpp>
#include <device/logger.hpp>

namespace mgmt::device {
  namespace details {
    static inline auto new_id()
    {
      static std::atomic<DeviceId_t> id { 1 };

      return id++;
    }
  }

  template<class D>
  inline auto inventory = Inventory<D>{};

  template<class D>
  auto register_device(D&& device)
  {
    auto id = details::new_id();

    common::logger::get(mgmt::device::Logger)->debug("{}, device id: {}", __FUNCTION__, id);

    inventory<D>.add(id, std::forward<D>(device));

    return id;
  }

  template<class D, class... Args>
  auto register_device(Args&&...args)
  {
    auto id = details::new_id();

    common::logger::get(mgmt::device::Logger)->debug("{}, device id: {}", __FUNCTION__, id);

    inventory<D>.emplace(id, std::forward<Args>(args)...);

    return id;
  }

  template<class D>
  decltype(auto) get_device(const DeviceId_t& id)
  {
    return inventory<D>.get(id);
  }

  template<class D>
  auto deregister_device(const DeviceId_t& id)
  {
    common::logger::get(mgmt::device::Logger)->debug("{}, device id: {}", __FUNCTION__, id);

    inventory<D>.remove(id);
  }
}