#pragma once

#include <hw/drivers/driver.hpp>

namespace hw::platform_device
{
  template<class DriverInterface, class Loader>
  constexpr bool is_loader_binary_compatible_match()
  {
    using Compatible_t = typename Loader::Compatible_t;

    return std::is_same_v<DriverInterface, Compatible_t> ||
          std::is_base_of_v<DriverInterface, Compatible_t>;
  }

  template<class Loader>
  bool is_loader_string_compatible_match(const std::string& compatible)
  {
    return Loader::compatible() == compatible;
  }

  template<common::traits::IsTupleLike TLoaders, class TDevManager>
  class ProbeContext
  {
    using Loaders_t = TLoaders;
    using DevManager_t = TDevManager;
  public:
    explicit ProbeContext(DevManager_t& devm)
      : devm_{devm}
    {}

    template<class...Args>
    auto register_device(Args&&...args)
    {
      return devm_.register_device(std::forward<Args>(args)...);
    }

    template<class DriverInterface = hw::drivers::Driver>
    DriverInterface* probe(const PdTreeObject_t& device_description)
    {
      auto driver_out = static_cast<DriverInterface*>(nullptr);

      common::traits::TupleForEachType<Loaders_t>::invoke([this, &driver_out, &device_description]<class Loader>(common::traits::TypeTag<Loader>){
        if constexpr (not is_loader_binary_compatible_match<DriverInterface, Loader>()) {
          return;
        } else {
          driver_out = probe_driver<DriverInterface, Loader>(device_description);
        }
      });

      return driver_out;
    }

  private:
    template<class DriverInterface, class Loader>
    DriverInterface* probe_driver(const PdTreeObject_t& device_description)
    {
      const auto loader_compatible_match = pdtree_to_string(device_description.at("compatible"));

      if (not is_loader_string_compatible_match<Loader>(loader_compatible_match)) {
        return nullptr;
      }

      return Loader::probe(*this, device_description);
    }
  private:
    DevManager_t& devm_;
  };
}