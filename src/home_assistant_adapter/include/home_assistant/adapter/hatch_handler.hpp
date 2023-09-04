#pragma once

#include <utility>

#include <home_assistant/mqtt/coverv2.hpp>
#include <home_assistant/mqtt/device_identity.hpp>
#include <home_assistant/adapter/device_identity_provider.hpp>
#include <home_assistant/adapter/entity_factory.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::adapter {

class HatchHandler
{
public:
  // movable
  HatchHandler(HatchHandler&& rhs) noexcept = default;
  HatchHandler& operator=(HatchHandler&& rhs) noexcept = default;
  // non-copyable
  HatchHandler(const HatchHandler&) = delete;
  HatchHandler& operator=(const HatchHandler&) = delete;

  ~HatchHandler() = default;

  static boost::asio::awaitable<std::optional<HatchHandler>> async_create(
    mgmt::device::DeviceId_t device_id,
    const DeviceIdentityProvider& identity_provider,
    const EntityFactory& factory
  );

  mgmt::device::DeviceId_t hardware_id() const;
  boost::asio::awaitable<void> async_run();
  boost::asio::awaitable<void> async_sync_state();

private:
  HatchHandler(mgmt::device::DeviceId_t device_id, Cover_t cover, v2::DeviceIdentity device_identity);
  boost::asio::awaitable<bool> async_configure();
  boost::asio::awaitable<void> async_handle_command(const v2::CoverCommand& command);
  boost::asio::awaitable<void> async_handle_error(const v2::Error& error);

private:
  mgmt::device::DeviceId_t device_id_;
  Cover_t cover_;
  v2::DeviceIdentity device_identity_;
};
}// namespace mgmt::home_assistant::device
