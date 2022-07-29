//
// Created by pp on 7/24/22.
//

#include <algorithm>

#include <home_assistant/device/hatch_event_handler.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::device {
HatchEventHandler::HatchEventHandler(
  const EntityFactory& factory,
  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }, device_identity_provider_{ device_identity_provider }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> HatchEventHandler::operator()([[maybe_unused]] const DeviceCreated_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceCreated)", __FUNCTION__);

  hatches_.emplace_back(event.device_id, device_identity_provider_, factory_);

  auto& h = hatches_.back();
  h.connect();
  co_return;
}

boost::asio::awaitable<void> HatchEventHandler::operator()([[maybe_unused]] const DeviceRemoved_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceRemoved)", __FUNCTION__);

  const auto device_id = event.device_id;
  const auto erased = std::erase_if(hatches_, [device_id](auto& h) {
    return device_id == h.hardware_id();
  });

  if (not erased) {
    throw std::runtime_error(fmt::format("Hatch device with id: {} was not found", device_id));
  }

  co_return;
}

boost::asio::awaitable<void> HatchEventHandler::operator()([[maybe_unused]] const DeviceStateChanged_t& event)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}(DeviceStateChanged)", __FUNCTION__);

  const auto device_id = event.device_id;
  auto hatch = std::ranges::find_if(hatches_, [device_id](auto& h) {
    return device_id == h.hardware_id();
  });

  if (hatch == hatches_.end()) {
    throw std::runtime_error(fmt::format("Hatch device with id: {} was not found", device_id));
  }

  hatch->async_sync_state();

  co_return;
}
void HatchEventHandler::on_error()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchEventHandler::{}", __FUNCTION__);
}
}// namespace mgmt::home_assistant::device
