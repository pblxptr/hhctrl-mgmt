#pragma once

#include <unordered_map>
#include <string>

namespace mgmt::platform_device
{
  using DeviceAttributes_t = std::unordered_map<std::string, std::string>;
}