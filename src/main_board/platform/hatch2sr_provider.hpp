#pragma once

#include <main_board/platform/device_loader.hpp>
#include <main_board/platform/pdtree.hpp>
#include <main_board/device/sysfs_hatch.hpp>
#include <device/device_register.hpp>

namespace mgmt::platform_device
{
  class HatchProvider
  {
  public:
    constexpr auto compatible() const
    {
      return "sysfs_hatch2sr";
    }

    template<class BoardBuilder>
    bool load(BoardBuilder& builder, const PdTreeObject_t& object)
    {
      constexpr auto sysfs_path_atrr = "sysfs_path";
      constexpr auto model_attr = "model";

      common::logger::get(mgmt::device::Logger)->debug("Hatch2srDriverLoader: probe driver '{}'", pdtree_to_string(object.at(model_attr)));

      if (not object.contains("sysfs_path")) {
        common::logger::get(mgmt::device::Logger)->error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
        return false;
      } 

      auto sysfs_path = pdtree_get<std::string>(object, sysfs_path_atrr);

      builder.template add_loader<mgmt::device::SysfsHatch>(DeviceLoader {
        .load = [sysfs_path = std::move(sysfs_path)]() {
          return mgmt::device::register_device<mgmt::device::SysfsHatch>(sysfs_path);
        },
        .unload = [](int ) {
          throw std::runtime_error("Not supported");
         }
      });

      return true;
    }
  };
}