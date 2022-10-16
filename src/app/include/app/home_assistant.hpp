#pragma once

#include <boost/asio/io_context.hpp>

#include <app/app_config.hpp>
#include <event/event_bus.hpp>
#include <device/devicetree.hpp>
#include <device/hardware_identity_store.hpp>


namespace mgmt::app
{
  struct HomeAssistantServices
  {
    boost::asio::io_context& context;
    mgmt::device::DeviceTree& dtree;
    mgmt::device::HardwareIdentityStore_t& hw_identity_store;
    common::event::AsyncEventBus& bus;
    const mgmt::app::AppConfig& config;
  };

  void home_assistant_init(const HomeAssistantServices& services);
} // namespace mgmt::app
