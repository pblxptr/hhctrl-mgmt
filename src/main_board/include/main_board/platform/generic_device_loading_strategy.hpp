#pragma once

#include <device/device_id.hpp>
#include <device/devicetree.hpp>
#include <event/device_created.hpp>

namespace mgmt::platform_device {
class DefaultGenericDeviceLoadingStrategy
{
public:
  DefaultGenericDeviceLoadingStrategy(
    mgmt::device::DeviceId_t board_id,
    mgmt::device::DeviceTree& dtree,
    common::event::AsyncEventBus& bus)
    : board_id_{ board_id }, dtree_{ dtree }, bus_{ bus }
  {}

  template<class D, class Loader>
  bool handle(Loader&& loader) const
  {
    common::logger::get(mgmt::device::Logger)->debug("Loading generic device under board");

    auto device_id = loader.load();
    dtree_.add_child(board_id_, device_id);
    bus_.publish(mgmt::event::DeviceCreated<D>{
      device_id });

    return true;
  }

private:
  mgmt::device::DeviceId_t board_id_;
  mgmt::device::DeviceTree& dtree_;
  common::event::AsyncEventBus& bus_;
};
}// namespace mgmt::platform_device