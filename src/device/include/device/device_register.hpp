#pragma once

#include <atomic>
#include <spdlog/spdlog.h>

#include "inventory.hpp"
#include "device_id.hpp"
#include "logger.hpp"

namespace mgmt::device {
namespace details {
  static inline auto new_id()
  {
    static std::atomic<DeviceId_t> device_id{ 1 };

    return device_id++;
  }
}// namespace details

//TODO(pp): Move Invertory to invertory.hpp
template<class D>
inline auto Inventory = DeviceInventory<D>{}; //NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

template<class D>
auto register_device(D&& device)
{
  auto device_id = details::new_id();

  common::logger::get(mgmt::device::Logger)->debug("{}, device device_id: {}", __FUNCTION__, device_id);

  Inventory<D>.add(device_id, std::forward<D>(device));

  return device_id;
}

template<class D, class... Args>
auto register_device(Args&&... args)
{
  auto device_id = details::new_id();

  common::logger::get(mgmt::device::Logger)->debug("{}, device device_id: {}", __FUNCTION__, device_id);

  Inventory<D>.emplace(device_id, std::forward<Args>(args)...);

  return device_id;
}

template<class D>
decltype(auto) get_device(const DeviceId_t& device_id)
{
  return Inventory<D>.get(device_id);
}

template<class D>
auto deregister_device(const DeviceId_t& device_id)
{
  common::logger::get(mgmt::device::Logger)->debug("{}, device device_id: {}", __FUNCTION__, device_id);

  Inventory<D>.remove(device_id);
}
}// namespace mgmt::device
