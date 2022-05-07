#include <string>

#include <newnew/platform_device_loader.hpp>

namespace mgmt::platform_device
{
  class DeviceScanner
  {
  public:
    using DeviceLoader_t = std::unique_ptr<DeviceLoader>;

    virtual ~DeviceScanner() = default;
    DeviceScanner(const std::string&, std::vector<DeviceLoader_t>);
    void scan(mgmt::device::Device&);
  private:
    std::string path_;
    std::vector<DeviceLoader_t> loaders_;
  };
}
