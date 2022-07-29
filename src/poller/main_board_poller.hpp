//
// Created by pp on 7/27/22.
//

#pragma once

#include <boost/functional/hash.hpp>
#include <boost/asio/awaitable.hpp>

#include <main_board/device/main_board.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>
#include <common/event/event_bus.hpp>
#include <events/device_state_changed.hpp>
#include <poller/logger.hpp>

namespace mgmt::poller
{
  class MainBoardPoller {
  public:
    MainBoardPoller(mgmt::device::DeviceId_t device_id, common::event::AsyncEventBus &bus)
      : device_id_{std::move(device_id)}
      , bus_{bus}
      , hash_{}
    {
    }

    boost::asio::awaitable<void> operator()()
    {
      const auto tmp_hash = current_hash();
      if (tmp_hash != hash_) {
        update(tmp_hash);
      }

      common::logger::get(mgmt::poller::Logger)->debug("Polling MainBoardDevice({}): {}",
        device_id_,
        tmp_hash == hash_ ? "up to date" : "changed"
      );

      co_return;
    }
  private:
    size_t current_hash() const
    {
      const auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);
      auto tmp_hash = size_t{0};
      boost::hash_combine(tmp_hash, board.indicator_state(mgmt::device::IndicatorType::Fault));
      boost::hash_combine(tmp_hash, board.indicator_state(mgmt::device::IndicatorType::Maintenance));
      boost::hash_combine(tmp_hash, board.indicator_state(mgmt::device::IndicatorType::Warning));
      boost::hash_combine(tmp_hash, board.indicator_state(mgmt::device::IndicatorType::Status));

      return tmp_hash;
    }

    void update(std::size_t new_hash)
    {
      hash_ = new_hash;
      bus_.publish(mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>(device_id_));
    }

  private:
    mgmt::device::DeviceId_t device_id_;
    common::event::AsyncEventBus& bus_;
    std::size_t hash_;
  };

}
