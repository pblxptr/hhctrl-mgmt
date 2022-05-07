#include <new/platform_device/loaders/sysfs_rgbled_loader.hpp>
#include <new/device/led/rgb3_led.hpp>
#include <new/device/led/sysfs_led.hpp>

#include <string_view>

namespace {
  constexpr auto SysfsPathAttr { "sysfs_path" };
  constexpr auto ColorAttr { "color" };

  std::unique_ptr<mgmt::device::Led> load_led(const mgmt::platform_device::PdTreeArray_t& leds, std::string_view led_label)
  {
    using namespace mgmt::platform_device;

    auto led_descriptor = std::find_if(leds.begin(), leds.end(), [&led_label](const auto& e) {
        const auto& e_obj = e.as_object();
        return e_obj.contains(ColorAttr) && pdtree_to_string(e_obj.at(ColorAttr)) == led_label;
      });

    if (led_descriptor == leds.end()) {
      return nullptr;
    }
    return std::make_unique<mgmt::device::SysfsLed>(
      mgmt::device::DeviceId::new_id(),
      pdtree_get<std::string>(led_descriptor->as_object(), SysfsPathAttr)
    );
  }
}

namespace mgmt::platform_device
{
  bool SysfsRGBLedLoader::is_compatible(const std::string& compatible) const
  {
    return compatible == "sysfs_rgb_led";
  }

  void SysfsRGBLedLoader::load(mgmt::device::Device& parent, const PdTreeObject_t& descriptor) const
  {
      const auto& leds = descriptor.at("leds").as_array();
      auto red =   load_led(leds, "red");
      auto green = load_led(leds, "green");
      auto blue =  load_led(leds, "blue");

      if (not red || not green || not blue) {
        throw std::runtime_error("Driver cannot be loaded. One of its dependecies is not configured properly");
      }

      auto rgbled = std::make_unique<mgmt::device::RGB3Led>(
        mgmt::device::DeviceId::new_id(),
        std::move(red),
        std::move(green),
        std::move(blue)
      );
  }
}