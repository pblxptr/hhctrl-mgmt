#pragma once

#include <event/device_created.hpp>
#include <event/device_removed.hpp>
#include <event/device_state_changed.hpp>

#include <home_assistant/entity_factory.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/device/main_board_handler.hpp>
#include <main_board/device/main_board.hpp>

namespace mgmt::home_assistant::device {
class MainBoardEventHandler
{
  using DeviceCreated_t = mgmt::event::DeviceCreated<mgmt::device::MainBoard>;
  using DeviceRemoved_t = mgmt::event::DeviceRemoved<mgmt::device::MainBoard>;
  using DeviceStateChanged_t = mgmt::event::DeviceStateChanged<mgmt::device::MainBoard>;

public:
  MainBoardEventHandler(
    const EntityFactory& factory,
    const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider);
  boost::asio::awaitable<void> operator()(const DeviceCreated_t& event);
  boost::asio::awaitable<void> operator()(const DeviceRemoved_t& event);
  boost::asio::awaitable<void> operator()(const DeviceStateChanged_t& event);

private:
  EntityFactory factory_;
  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider_;
  std::optional<MainBoardHandler> main_board_;
};

}// namespace mgmt::home_assistant::device
