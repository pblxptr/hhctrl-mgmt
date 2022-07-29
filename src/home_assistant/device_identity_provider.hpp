//
// Created by pp on 7/23/22.
//

#pragma once

#include <string>
#include <stdexcept>
#include <fmt/format.h>

#include <device/device_id.hpp>
#include <device/devicetree.hpp>
#include <device/hardware_identity_store.hpp>
#include <device/hardware_identity.hpp>
#include <home_assistant/device_identity.hpp>

namespace mgmt::home_assistant {
class DeviceIdentityProvider
{
public:
  DeviceIdentityProvider(const mgmt::device::HardwareIdentityStore_t& store, const mgmt::device::DeviceTree& dtree)
    : hw_identity_{ store }, dtree_{ dtree }
  {}

  [[nodiscard]] DeviceIdentity identity(const mgmt::device::DeviceId_t& device_id) const
  {
    auto hw_identity = get_hardware_identity(device_id);

    return DeviceIdentity{
      .manufacturer = std::move(hw_identity.manufacturer),
      .model = std::move(hw_identity.model),
      .serial_number = std::move(hw_identity.serial_number),
      .hw_revision = std::move(hw_identity.revision),
      .sw_revision = "Sw rev default"
    };
  }

private:
  [[nodiscard]] mgmt::device::HardwareIdentity get_hardware_identity(const mgmt::device::DeviceId_t& device_id) const
  {
    auto do_get_hardware_identity = [this](const auto& dev_id) {
      if (not hw_identity_.contains(dev_id)) {
        throw std::runtime_error("Cannot get HardwreIdentity for requested device id");
      }
      return hw_identity_.at(dev_id);
    };

    if (hw_identity_.contains(device_id)) {
      return do_get_hardware_identity(device_id);
    } else {
      const auto parent_id = dtree_.parent(device_id);
      if (not parent_id) {
        throw std::runtime_error("Cannot get DeviceInfo - parent does not exist");
      }
      return do_get_hardware_identity(*parent_id);
    }
  }

private:
  const mgmt::device::HardwareIdentityStore_t& hw_identity_;
  const mgmt::device::DeviceTree& dtree_;
};
}// namespace mgmt::home_assistant