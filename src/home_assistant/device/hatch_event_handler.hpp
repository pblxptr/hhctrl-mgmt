#pragma once

#include <boost/asio/awaitable.hpp>
#include <home_assistant/entity_factory.hpp>
#include <device/hatch_t.hpp>
#include <events/device_created.hpp>
#include <events/device_removed.hpp>
#include <events/device_state_changed.hpp>
#include <home_assistant/device/hatch_handler.hpp>

namespace mgmt::home_assistant::device
{
  class HatchEventHandler
  {
    using DeviceCreated_t = mgmt::event::DeviceCreated<mgmt::device::Hatch_t>;
    using DeviceRemoved_t = mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>;
    using DeviceStateChanged_t = mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>;
  public:
    explicit HatchEventHandler(const EntityFactory& factory)
      : factory_{factory}
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}", __FUNCTION__);

      hatches_.reserve(10);
    }
 
    boost::asio::awaitable<void> operator()([[maybe_unused]] const DeviceCreated_t& event)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceCreated)", __FUNCTION__);

      auto unique_id = "q1231asd_uid";

      hatches_.push_back(HatchHandler{event.device_id, factory_.create_cover(unique_id)});

      auto& h = hatches_.back();
      h.connect();
      co_return;
    }

    boost::asio::awaitable<void> operator()([[maybe_unused]] const DeviceRemoved_t& event)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceRemoved)", __FUNCTION__);

      co_return;
    }

    boost::asio::awaitable<void> operator()([[maybe_unused]] const DeviceStateChanged_t& event)
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceStateChanged)", __FUNCTION__);

      co_return;
    }
  private:
    void on_error()
    {
      common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}", __FUNCTION__);
    }
  private:
    const EntityFactory& factory_;
    std::vector<HatchHandler> hatches_;
  };
}