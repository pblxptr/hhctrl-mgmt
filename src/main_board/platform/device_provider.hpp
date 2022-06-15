#pragma once

#include <concepts>
#include <device/device_id.hpp>

namespace mgmt::platform_device
{
  template<class T>
  concept Loader = requires(T v)
  {
    { v.compatible() } -> std::same_as<const char*>;
    { v.load(std::declval<const mgmt::device::DeviceId_t&>()) } -> std::same_as<bool>;
  };
}