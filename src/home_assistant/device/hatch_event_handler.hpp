#pragma once

#include <boost/asio/awaitable.hpp>

#include <device/hatch_t.hpp>
#include <events/device_created.hpp>
#include <events/device_removed.hpp>
#include <events/device_state_changed.hpp>
#include <home_assistant/device/hatch_handler.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/entity_factory.hpp>


namespace mgmt::home_assistant::device {
class HatchEventHandler
{
  using DeviceCreated_t = mgmt::event::DeviceCreated<mgmt::device::Hatch_t>;
  using DeviceRemoved_t = mgmt::event::DeviceRemoved<mgmt::device::Hatch_t>;
  using DeviceStateChanged_t = mgmt::event::DeviceStateChanged<mgmt::device::Hatch_t>;

public:
  HatchEventHandler(
    const EntityFactory& factory,
    const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider);
  boost::asio::awaitable<void> operator()(const DeviceCreated_t& event);
  boost::asio::awaitable<void> operator()(const DeviceRemoved_t& event);
  boost::asio::awaitable<void> operator()(const DeviceStateChanged_t& event);

private:
  void on_error();

private:
  const EntityFactory& factory_;
  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider_;
  std::vector<HatchHandler> hatches_;
};
}// namespace mgmt::home_assistant::device
