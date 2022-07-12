#pragma once

#include <vector>
#include <string>

namespace mgmt::home_assistant::mqttc
{
  struct Device
  {
    std::vector<std::string> identifiers;
    std::string manufacturer;
    std::string name;
    std::string model;
    std::string sw_version;
  };
}
