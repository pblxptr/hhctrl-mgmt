#pragma once

#include <hw/platform_device/pd_device_loader.hpp>
#include <hw/platform_device/pdtree.hpp>
#include <hw/drivers/driver.hpp>


namespace hw::platform_device
{
  class PlatformDeviceLoader;


  class DriverLoader
  {
  public:
    virtual ~DriverLoader() = default;
    bool probe(const PdTreeObject_t& object) const
    {
      return probe(object);
    }
    virtual constexpr std::string_view compatible() const = 0;
  };

  template<class TDriver>
  class BaseDriverLoader : public DriverLoader
  {
  public:
    virtual TDriver* probe(const PdTreeObject_t&) = 0;

  protected:
    void register_driver(std::unique_ptr<TDriver> driver, bool visible = true)
    {
      //auto [driver_id, driver_ptr] = generate_id();
      
      // if (visible) {
      //   pd_ctrl_adapter.add_compatible_driver_id<TDriver>(driver_id);
      // }
    }
  };
}