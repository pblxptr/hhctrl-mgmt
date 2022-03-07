#pragma once

#include <hw/platform_device/driver_loader.hpp>
#include <hw/platform_device/pdtree.hpp>
#include <hw/drivers/driver.hpp>

#include <vector>
#include <string_view>
#include <memory>

namespace hw::platform_device
{
class PlatformDeviceLoader
{
public:
template<class T>
void add_driver_loader()
{
  loaders_.push_back(std::make_unique<T>());
}
DriverLoader* find_loader(const std::string_view) const;
DriverLoader* find_loader(const hw::platform_device::PdTreeObject_t&) const;
void load(const std::string&);

private:
  std::vector<std::unique_ptr<DriverLoader>> loaders_;
};
}