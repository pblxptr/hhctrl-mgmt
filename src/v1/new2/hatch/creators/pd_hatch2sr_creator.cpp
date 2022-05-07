#include <new/platform_device/creator/device_creator.hpp>
#include <new/device/misc/sysfs_hatch.hpp>
#include <new2/hatch/services/hatch_pooler.hpp>

namespace {
  constexpr auto SysfsPathAttr = "sysfs_path";
}

namespace mgmt::platform_device
{
  bool PlatformDeviceHatch2SRCreator::is_compatible(const std::string& compatible) const
  {
    return compatible == "sysfs_hatch2sr";
  }

  void PlatformDeviceHatch2SRCreator::create(mgmt::device::Device& parent, const PdTreeObject_t& descriptor) const
  {
    if (not descriptor.contains(SysfsPathAttr)) {
      throw std::runtime_error("Missing attribute 'sysfs_path' id pdtree for hatch2sr driver descriptor");
    }

    auto hatch_context.create(parent);
  }
}


struct HatchContext
{
  template<class T, class...Args>
  void create(Device& parent, Args&&... args)
  {
    auto hatch = make_trackable_device<T>(std::forward<Args>(args)...,
      [this](auto& dev){ remove(dev); }
    );

    configure(hatch);
    auto event = 
  }
}

struct HatchCreated
{
  DeviceId device_id;
}

store = ReadonlyStore<Hatch>
store.get_device(device_id);

/**
 * - device
 *   - hatch
 *   - led
 *   - rgbled
 *  - pooler
 *  - platform_device_scanner
 *    - events
 *        - handlers
 *          - main_board_creted
 *    - scanner
 *    - 
 * 
 * 
 * 
 * 
 *  HatchFactory->create<>;
 * 
 */

// - device_creators
//   - hatch