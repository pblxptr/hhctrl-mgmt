#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

namespace mgmt::device
{
  class Device
  {
  public:
    using DeviceId_t = std::string;

    Device(DeviceId_t device_id)
      : id_{std::move(device_id)}
    {}
    virtual ~Device() = default;

    Device(const Device&) = default;
    Device& operator=(const Device&) = default;

    Device(Device&&) = default;
    Device& operator=(Device&&) = default;

    DeviceId_t id() const
    {
      return id_;
    }

    void add_device(std::unique_ptr<Device> device)
    {
      device->set_parent(this);
      devices_.push_back(std::move(device));
    }

    void set_parent(Device* parent)
    {
      parent_ = parent;
    }

    Device* parent() const
    {
      return parent_;
    }

  private:
    DeviceId_t id_ {};
    Device* parent_ { nullptr };
    std::vector<std::unique_ptr<Device>> devices_ {};
  };
}