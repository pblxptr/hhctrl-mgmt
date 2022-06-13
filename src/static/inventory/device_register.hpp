#pragma once

#include <atomic>
#include <spdlog/spdlog.h>

#include <static/inventory/inventory.hpp>
#include <static/device/device_id.hpp>

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

    spdlog::get("mgmt")->info("Register device with id: {}", id);

    inventory<D>.add(id, std::forward<D>(device));

    return id;
  }

  template<class D, class... Args>
  auto register_device(Args&&...args)
  {
    auto id = details::new_id();

    spdlog::get("mgmt")->debug("Register (emplace) device with id: {}", id);

    inventory<D>.emplace(id, std::forward<Args>(args)...);

    return id;
  }

  template<class D>
  auto deregister_device(const DeviceId_t& id)
  {
    spdlog::get("mgmt")->debug("Deregister device with id: {}", id);

    inventory<D>.remove(id);
  }
}