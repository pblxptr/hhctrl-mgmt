//
// Created by pp on 7/27/22.
//

#pragma once

#include <boost/functional/hash.hpp>
#include <boost/asio/awaitable.hpp>

#include <device/hatch_t.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>
#include <event/event_bus.hpp>
#include <event/device_state_changed.hpp>
#include <poller/logger.hpp>

namespace mgmt::poller {
class HatchPoller
{
public:
  HatchPoller(mgmt::device::DeviceId_t device_id, common::event::AsyncEventBus& bus)
    : device_id_{ std::move(device_id) }
    , bus_{ bus }
  {
  }

  boost::asio::awaitable<void> operator()()
  {
    const auto tmp_hash = current_hash();

    common::logger::get(mgmt::poller::Logger)->debug("Polling Hatch({}): {}", device_id_, tmp_hash == hash_ ? "up to date" : "changed");

    if (tmp_hash != hash_) {
      update(tmp_hash);
    }
    co_return;
  }

private:
  size_t current_hash() const
  {
    using mgmt::device::Hatch;

    const Hatch auto& hatch = mgmt::device::get_device<mgmt::device::Hatch_t>(device_id_);
    const auto hatch_status = hatch.status();
    const std::size_t tmp_hash = std::hash<decltype(hatch_status)>{}(hatch_status);

    return tmp_hash;
  }

  void update(std::size_t new_hash)
  {
    hash_ = new_hash;
    bus_.publish(mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>(device_id_));
  }

private:
  mgmt::device::DeviceId_t device_id_;
  common::event::AsyncEventBus& bus_;
  std::size_t hash_{};
};

}// namespace mgmt::poller
