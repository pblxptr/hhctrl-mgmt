#pragma once

#include <spdlog/spdlog.h>

#include <hw/platform_device/driver_loader.hpp>
#include <hw/drivers/misc/sysfs_hatch.hpp>

namespace hw::platform_device
{
class Hatch2srDriverLoader : public BaseDriverLoader<hw::drivers::SysfsHatchDriver>
{
public:
  constexpr std::string_view compatible() const override
  {
    return "sysfs_hatch2sr";
  }
  hw::drivers::SysfsHatchDriver* probe(const PdTreeObject_t& object) override
  {
    spdlog::get("hw")->info("Hatch2srDriverLoader: probe");

    if (not object.contains("sysfs_path")) {
      spdlog::get("hw")->error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
      return nullptr;
    }

    //register_driver(..., false);
    //return register_driver(std::unique_ptr<>, true);

    // const auto driver_id = allocate_driver<hw::drivers::SysfsHatchDriver>(...);
    // enable_direct_control(driver_id);

    // auto driver_id = pd.register_driver();
    // pd.enable_ctrl_adapter<SysfsHatchDriver>(driver_id);

    // return std::make_unique<hw::drivers::SysfsHatchDriver>(object.at("sysfs_path").as_string().c_str());
    return nullptr;
  }
};
}