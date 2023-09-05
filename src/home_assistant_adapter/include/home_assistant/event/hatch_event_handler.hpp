#pragma once

#include <boost/asio/awaitable.hpp>
#include <list>

#include <device/hatch_t.hpp>
#include <event/device_created.hpp>
#include <event/device_removed.hpp>
#include <event/device_state_changed.hpp>
#include <home_assistant/adapter/hatch_handler.hpp>
//#include <home_assistant/adapter/device_identity_provider.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/adapter/entity_factory.hpp>

namespace mgmt::home_assistant::event {
class HatchEventHandler
{
  using DeviceCreated_t = mgmt::event::DeviceCreated<mgmt::device::Hatch_t>;
  using DeviceRemoved_t = mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>;
  using DeviceStateChanged_t = mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>;

public:
  HatchEventHandler(
    const adapter::EntityFactory& factory,
    const DeviceIdentityProvider& device_identity_provider);
  boost::asio::awaitable<void> operator()(const DeviceCreated_t& event);
  boost::asio::awaitable<void> operator()(const DeviceRemoved_t& event);
  boost::asio::awaitable<void> operator()(const DeviceStateChanged_t& event);

private:
  const adapter::EntityFactory& factory_;
  const DeviceIdentityProvider& device_identity_provider_;
  std::list<adapter::HatchHandler> hatches_;
};
}// namespace mgmt::home_assistant::device
