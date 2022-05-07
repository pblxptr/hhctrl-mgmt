#include <new/platform_device/loaders/hatch2sr_loader.hpp>
#include <new/device/misc/sysfs_hatch.hpp>

namespace {
  constexpr auto SysfsPathAttr = "sysfs_path";
}

namespace mgmt::platform_device
{
  bool Hatch2SRLoader::is_compatible(const std::string& compatible) const
  {
    return compatible == "sysfs_hatch2sr";
  }

  void Hatch2SRLoader::load(mgmt::device::Device& parent, const PdTreeObject_t& descriptor) const
  {
    if (not descriptor.contains(SysfsPathAttr)) {
      throw std::runtime_error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
    }

    auto hatch = std::make_unique<mgmt::device::SysfsHatch>(
      mgmt::device::DeviceId::new_id(),
      pdtree_get<std::string>(descriptor, SysfsPathAttr)
    );

    parent.add_device(std::move(hatch));
  }
}