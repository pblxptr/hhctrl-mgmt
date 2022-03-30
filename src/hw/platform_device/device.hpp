#pragma once

#include <string>

#include <spdlog/spdlog.h>
#include <hw/platform_device/device_attributes.hpp>

namespace hw::platform_device
{
  class Device
  {
  public:
    using DeviceId_t = std::string;

    virtual ~Device() = default;
    Device(const Device&) = default;
    Device(Device&&) = default;
    Device& operator=(const Device&) = default;
    Device& operator=(Device&&) = default;

    Device(DeviceId_t devid, DeviceAttributes attrs)
      : device_id_{std::move(devid)}
      , attributes_{std::move(attrs)}
    {
      spdlog::get("hw")->debug("Create device with id: {}", device_id_);
    }

    const DeviceId_t& id() const
    {
      return device_id_;
    }

    template<class T>
    decltype(auto) attribute(const std::string& key) const
    {
      return attributes_.attribute<T>(key);
    }

    template<class T>
    decltype(auto) find_attribute(const std::string& key)
    {
      return attributes_.find_attribute<T>(key);
    }

    decltype(auto) attributes() const
    {
      return attributes_;
    }
  private:
    DeviceId_t device_id_;
    DeviceAttributes attributes_;
  };

  template<class DriverInterface>
  class GenericDevice : public Device
  {
  public:
    GenericDevice(DeviceId_t devid, DeviceAttributes attrs, std::unique_ptr<DriverInterface> driver) 
      : Device(std::move(devid), std::move(attrs))
      , driver_{std::move(driver)}
    {}

    DriverInterface* driver() const //Todo: Consider changing to ref??
    {
      return driver_.get();
    }

  private:
    std::unique_ptr<DriverInterface> driver_;
  };
}