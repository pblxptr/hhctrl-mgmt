//
// Created by pp on 7/24/22.
//

#pragma once

namespace mgmt::home_assistant {
struct DeviceIdentity
{
  std::string manufacturer;
  std::string model;
  std::string serial_number;
  std::string hw_revision;
  std::string sw_revision;
};
}// namespace mgmt::home_assistant