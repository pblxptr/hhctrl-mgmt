#include <new/platform_device/loaders/sysfs_led_loader.hpp>
#include <new/device/led/sysfs_led.hpp>

namespace {
  constexpr auto SysfsPathAttr = "sysfs_path";
}

namespace mgmt::platform_device
{
  bool SysfsLedLoader::is_compatible(const std::string& compatible) const
  {
    return compatible == "sysfs_led";
  }

  void SysfsLedLoader::load(mgmt::device::Device& parent, const PdTreeObject_t& descriptor) const
  {
    if (not descriptor.contains(SysfsPathAttr)) {
      throw std::runtime_error("Missing attribute 'sysfs_path' id pdtree for sysfsled driver descriptor");
    }

    auto led = std::make_unique<mgmt::device::SysfsLed>(
      mgmt::device::DeviceId::new_id(),
      pdtree_get<std::string>(descriptor, SysfsPathAttr)
    );

    parent.add_device(std::move(led));
  }
}