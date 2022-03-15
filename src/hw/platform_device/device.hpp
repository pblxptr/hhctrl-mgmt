#pragma once

#include <string>

#include <spdlog/spdlog.h>
#include <hw/platform_device/device_attributes.hpp>

namespace hw::platform_device
{
  class DeviceBase //Todo: Change to Device, and device class that is below change to generic device
  {
  public:
    using DeviceId_t = std::string;

    DeviceBase(DeviceId_t devid, DeviceAttributes attrs)
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

  private:
    DeviceId_t device_id_;
    DeviceAttributes attributes_;
  };

  template<class DriverInterface>
  class Device : public DeviceBase
  {
  public:
    Device(DeviceId_t devid, DeviceAttributes attrs, std::unique_ptr<DriverInterface> driver) 
      : DeviceBase(std::move(devid), std::move(attrs))
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