//
// Created by pp on 7/27/22.
//

#pragma once

#include <device/device_id.hpp>
#include <common/event/event_bus.hpp>

namespace mgmt::poller {
class PollerFactory
{
public:
  explicit PollerFactory(common::event::AsyncEventBus& bus)
    : bus_{ bus }
  {}
  template<class Poller>
  auto create_poller(const mgmt::device::DeviceId_t& device_id)
  {
    return Poller{ device_id, bus_ };
  }

private:
  common::event::AsyncEventBus& bus_;
};
}// namespace mgmt::poller
