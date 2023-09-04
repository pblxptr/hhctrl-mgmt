//
// Created by pp on 7/24/22.
//

#pragma once

#include <string>

namespace mgmt::home_assistant::v2 {
struct DeviceIdentity
{
  std::string manufacturer;
  std::string model;
  std::string serial_number;
  std::string hw_revision;
  std::string sw_revision;
};
}// namespace mgmt::home_assistant
