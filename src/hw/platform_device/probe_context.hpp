#pragma once

#include <hw/drivers/driver.hpp>

namespace hw::platform_device
{
  template<class TLoaders, class TDevManager>
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
    auto probe(const PdTreeObject_t& object)
    {
      return find_and_probe<DriverInterface>(object, std::make_index_sequence<std::tuple_size_v<Loaders_t>>{});
    }

  private:
    template<class DriverInterface, size_t... Is>
    auto find_and_probe(const PdTreeObject_t& object, std::index_sequence<Is...>)
    {
      auto driver_ptr = static_cast<DriverInterface*>(nullptr);

      (do_find_and_probe<DriverInterface, Is>(object, driver_ptr) || ...);

      return driver_ptr;
    }

    template<class DriverInterface, size_t Idx>
    auto do_find_and_probe(const PdTreeObject_t& object, DriverInterface*& driver_ptr)
    {
      using SelectedLoader_t = std::tuple_element_t<Idx, Loaders_t>;
      using SelectedLoaderCompatible_t = typename SelectedLoader_t::Compatible_t;

      if constexpr (not (std::is_same_v<DriverInterface, SelectedLoaderCompatible_t> ||
            std::is_base_of_v<DriverInterface, SelectedLoaderCompatible_t>)) {
          return false;
      }
      else {
        if ((SelectedLoader_t::compatible() == object.at("compatible").as_string())) {
            driver_ptr = SelectedLoader_t::probe(*this, object);
          return driver_ptr != nullptr;
        }
        else {
          return false;
        }
      }
    }

  private:
    DevManager_t& devm_;
  };
}