#pragma once

#include <string>

namespace mgmt::device
{
  struct HardwareIdentity
  {
    std::string manufacturer;
    std::string model;
    std::string revision;
    std::string serial_number;
  };
}