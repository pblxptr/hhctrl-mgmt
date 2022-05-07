#pragma once

#include <string>
#include <compare>

namespace mgmt::device
{
  class DeviceId
  {
  public:
    using Value_t = std::string;

    DeviceId();

    static DeviceId new_id();

    Value_t value() const;

    auto operator<=>(const DeviceId&) const = default;

  private:
    explicit DeviceId(Value_t value);

  private:
    Value_t value_;
  };

  inline std::string to_string(const DeviceId& device_id)
  {
    return device_id.value();
  }
}