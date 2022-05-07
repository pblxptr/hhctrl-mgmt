#pragma once

#include <hw/drivers/misc/sysfs_hatch.hpp>

#include <spdlog/spdlog.h>
#include <string_view>

namespace hw::platform_device
{
  class Hatch2srDriverLoader
  {
  public:
    using Compatible_t = hw::drivers::HatchDriver;

    constexpr static std::string_view compatible()
    {
      return "sysfs_hatch2sr";
    }

    template<class Context>
    static Compatible_t* probe(Context& ctx, const PdTreeObject_t& object)
    {
      constexpr auto sysfs_path_atrr = "sysfs_path";
      constexpr auto model_attr = "model";

      spdlog::get("hw")->debug("Hatch2srDriverLoader: probe driver '{}'", pdtree_to_string(object.at(model_attr)));

      if (not object.contains("sysfs_path")) {
        spdlog::get("hw")->error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
        return nullptr;
      }

      return ctx.template register_device(
          std::make_unique<hw::drivers::SysfsHatchDriver>(pdtree_get<std::string>(object, sysfs_path_atrr)),
          DeviceAttributes {
            std::pair{ model_attr, pdtree_get<std::string>(object, model_attr) }
          }
      );
    }
  };
}
