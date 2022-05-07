#include <new/platform_device/services/device_scaner.hpp>
#include <filesystem>
#include <new/platform_device/pdtree/pdtree.hpp>

namespace fs = std::filesystem;

namespace mgmt::platform_device
{
  DeviceScanner::DeviceScanner(const std::string& path, std::vector<DeviceLoader_t> loaders)
    : path_{path}
    , loaders_{std::move(loaders)}
  {
    if (!fs::exists(path)) {
      throw std::runtime_error("Platform device tree file does not exist");
    }
  }

  void DeviceScanner::scan(mgmt::device::Device& parent_dev)
  {
    auto pdtree_content = pdtree_load(path_);

    for (const auto& entry : pdtree_content.as_array()) {
      const auto& descriptor = entry.as_object();
      const auto compatible = pdtree_to_string(descriptor.at("compatible"));
      auto loader = std::find_if(loaders_.begin(), loaders_.end(), [&compatible](const auto& l) {
        return l->is_compatible(compatible);
      });

      if (loader == loaders_.end()) {
        throw std::runtime_error(fmt::format("Cannot find loader for device with compatible: {}", compatible));
      }
      (*loader)->load(parent_dev, descriptor);
    }
  }
}
