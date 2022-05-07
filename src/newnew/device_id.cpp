#include <newnew/device_id.hpp>

#include <atomic>
#include <fmt/format.h>

namespace {
  std::atomic<size_t> ids_ { 1 };

  constexpr auto DefaultId = "00000000000";
}

namespace mgmt::device
{
  DeviceId DeviceId::new_id()
  {
    return DeviceId{fmt::format("dev-{}", ids_++)};
  }

  DeviceId::DeviceId()
    : value_{DefaultId}
  {}

  DeviceId::DeviceId(DeviceId::Value_t value)
    : value_{std::move(value)}
  {}

  DeviceId::Value_t DeviceId::value() const
  {
    return value_;
  }
}
