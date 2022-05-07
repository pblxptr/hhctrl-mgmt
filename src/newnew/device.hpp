#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <type_traits>

#include <common/utils/type_id.hpp>
#include <newnew/device_id.hpp>

namespace mgmt::device
{
  using DeviceId_t = DeviceId;
  using RuntimeDeviceType_t = size_t;

  template<class T>
  class GenericDevice;

  template<class T>
  RuntimeDeviceType_t runtime_device_type() requires (std::is_base_of_v<GenericDevice<T>, T> == true)
  {
    return common::utils::TypeId::value<T>();
  }

  struct DeviceIdentity
  {
    DeviceId_t id;
    RuntimeDeviceType_t type;
    auto operator<=>(const DeviceIdentity&) const = default;
  };

  template<class T>
  struct Token;

  class Device
  {
  public:
    virtual ~Device()
    {
      for (auto& dev : devices_) {
        dev->set_parent(nullptr);
      }
    }

  protected:
    template<class T>
    Device(DeviceIdentity identity, Token<T> token)
      : identity_{std::move(identity)}
    {}

  public:
    DeviceId_t id() const
    {
      return identity_.id;
    } 
    RuntimeDeviceType_t type() const
    {
      return identity_.type;
    }

    DeviceIdentity identity() const
    {
      return identity_;
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
    DeviceIdentity identity_;
    Device* parent_ { nullptr };
    std::vector<std::unique_ptr<Device>> devices_ {};
  };


  template<class T>
  class GenericDevice : public Device
  {
    friend T;
  private:
    explicit GenericDevice(DeviceId_t id)
      : Device{DeviceIdentity{id, runtime_device_type<T>()}, Token<T>{}}
    {}
  };

  template<class T>
  struct Token
  {
    friend GenericDevice<T>;

    private:
      Token() = default;
  };
}