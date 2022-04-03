#pragma once

#include <hw/platform_device/device_resource.hpp>
#include <common/traits/tuple_traits.hpp>

#include <utility>
#include <tuple>

namespace hw::platform_device
{
  //DeviceManager inherits from DeviceResource created for each supported driver.
  //E.g. for LedDriver and HatchDriver its definition is generated as follows:
  //DeviceManager : public DeviceResurce<LedDriver>, public DeviceResurce<HatchDriver> and so on.

  template<common::traits::IsTupleLike T, class = std::make_index_sequence<std::tuple_size_v<T>>>
  class DeviceManager;

  template<common::traits::IsTupleLike T, size_t...Idx>
  class DeviceManager<T, std::index_sequence<Idx...>> : public DeviceResource<std::tuple_element_t<Idx, T>>...
{
  using DeviceResource<std::tuple_element_t<Idx, T>>::register_dev_resource...;
  public:
    DeviceManager() = default;
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager(DeviceManager&&) = default;
    DeviceManager& operator=(const DeviceManager&) = delete;
    DeviceManager& operator=(DeviceManager&&) = default;

    template<class DriverInterface>
    decltype(auto) device(const std::string& device_id)
    {
      return as_device_resource<DriverInterface>().device(device_id);
    }

    template<class DriverInterface>
    decltype(auto) devices()
    {
      return as_device_resource<DriverInterface>().devices();
    }

    template<class Driver>
    decltype(auto) register_device(Driver&& driver, DeviceAttributes attrs = {})
    {
      spdlog::get("hw")->debug("DeviceManager: register_device");

      //TODO: Add checks that verify if device manager supports driver

      return register_dev_resource(generate_id(), std::forward<Driver>(driver), std::move(attrs));
    }
  private:
    auto generate_id() const
    {
      static size_t dev_id = 0;

      return std::to_string(dev_id++);
    }

    template<class DriverInterface>
    decltype(auto) as_device_resource()
    {
      return static_cast<DeviceResource<DriverInterface>&>(*this);
    }
  };
}