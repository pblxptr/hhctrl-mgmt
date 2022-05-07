#pragma once

#include <hw/platform_device/pdtree.hpp>
#include <hw/platform_device/probe_context.hpp>
#include <common/traits/tuple_traits.hpp>

namespace hw::platform_device
{
  template<class T>
  concept DriverLoaderCompatible = requires (T a)
  {
    typename T::Compatible_t;

    { T::compatible() };
  };

  template<common::traits::IsTupleLike Loaders, class DevManager>
  class DeviceLoaderCtrl
  {
    using DevManager_t = DevManager;
    using Loaders_t = Loaders;

  public:
    explicit DeviceLoaderCtrl(DevManager_t& devm)
      : devm_{devm}
    {}

    void load(const std::string& pd_file_path)
    {
      auto pdtree_content = load_pdtree_file(pd_file_path);
      const auto& pdtree_array = pdtree_content.as_array();
      auto context = ProbeContext<Loaders_t, DevManager_t>{devm_};

      spdlog::get("hw")->info("Platform devices to load: {}", pdtree_array.size());

      for (const auto& entry : pdtree_array) {
        const auto& device_descriptor = entry.as_object();
        const auto driver = context.probe(device_descriptor);

        if (not driver) {
          throw std::runtime_error("Cannot load device driver");
        }

        spdlog::get("hw")->info("Driver loaded successfully");
      }
    }
  private:
    DevManager_t& devm_;
  };

  template<class T, class U>
  DeviceLoaderCtrl(U&) -> DeviceLoaderCtrl<T, U>;
}