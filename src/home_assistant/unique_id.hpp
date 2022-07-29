//
// Created by pp on 7/24/22.
//

#pragma once

#include <fmt/format.h>
#include <string>

#include <device/device_id.hpp>
#include <home_assistant/device_identity.hpp>

namespace mgmt::home_assistant
{
  inline std::string get_unique_id(const mgmt::device::DeviceId_t& device_id, const mgmt::home_assistant::DeviceIdentity& identity)
  {
    return fmt::format("{}_{}", identity.serial_number, device_id);
  };
}