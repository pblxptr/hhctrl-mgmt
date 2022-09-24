//
// Created by pp on 7/19/22.
//

#pragma once

#include <unordered_map>

#include "device_id.hpp"
#include "hardware_identity.hpp"

namespace mgmt::device {
using HardwareIdentityStore_t = std::unordered_map<DeviceId_t, HardwareIdentity>;
} // namespace mgmt::device
