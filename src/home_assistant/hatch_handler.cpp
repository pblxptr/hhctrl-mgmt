//
// Created by pp on 7/24/22.
//

#include <home_assistant/device/hatch_handler.hpp>
#include <device/hatch_t.hpp>
#include <coro/async_wait.hpp>

namespace mgmt::home_assistant::device {
HatchHandler::HatchHandler(
  mgmt::device::DeviceId_t device_id,
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
  const mgmt::home_assistant::EntityFactory& factory)
  : device_id_{ std::move(device_id) }
  , identity_provider_{ identity_provider }
  , cover_{ factory.create_cover(get_unique_id(device_id_, identity_provider_.identity(device_id_))) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}, device id: {}", __FUNCTION__, device_id_);

  setup();
}

HatchHandler::HatchHandler(HatchHandler&& rhs) noexcept
  : device_id_{ std::move(rhs.device_id_) }
  , identity_provider_{ rhs.identity_provider_ }
  , cover_{ std::move(rhs.cover_) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::(HatchHandler&&)");

  setup();
}

HatchHandler& HatchHandler::operator=(HatchHandler&& rhs) noexcept
{
  device_id_ = std::move(rhs.device_id_);
  cover_ = std::move(rhs.cover_);

  setup();

  return *this;
}

mgmt::device::DeviceId_t HatchHandler::hardware_id() const
{
  return device_id_;
}

boost::asio::awaitable<void> HatchHandler::async_connect()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

  co_await cover_.async_connect();
}

boost::asio::awaitable<void> HatchHandler::async_sync_state()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

  const auto& hatch = mgmt::device::get_device<mgmt::device::Hatch_t>(device_id_);
  auto cover_state = mgmt::home_assistant::mqttc::CoverState{};

  switch (hatch.status()) {
  case mgmt::device::HatchState::Open:
    cover_state = mgmt::home_assistant::mqttc::CoverState::Open;
    break;
  case mgmt::device::HatchState::Closed:
    cover_state = mgmt::home_assistant::mqttc::CoverState::Closed;
    break;
  default:
    spdlog::error("State not handled");// TODO(pp): Error just for the purpose of tests
    co_return;
  }

  co_await cover_.async_set_state(cover_state);
}

void HatchHandler::setup()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

  cover_.set_ack_handler([this]() -> boost::asio::awaitable<void> { co_await async_set_config(); });
  cover_.set_error_handler([this](const auto& error_code) { on_error(error_code); });
  cover_.on_command([this](const auto& cmd) {
    handle_command(cmd);
  });
}

boost::asio::awaitable<void> HatchHandler::async_set_config()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

  auto config = mgmt::home_assistant::mqttc::EntityConfig{ cover_.unique_id() };
  config.set("name", "Cover");
  config.set("device_class", "door");
  config.set("device", mqttc::helper::entity_config_basic_device(identity_provider_.identity(device_id_)));
  config.set(mqttc::CoverConfig::PayloadStopKey, nullptr);// Stop command not implemented
  co_await cover_.async_set_config(std::move(config));

  // Wait until entity is configured on remote
  co_await common::coro::async_wait(std::chrono::seconds(1));

  co_await cover_.async_set_availability(mgmt::home_assistant::mqttc::Availability::Online);
  co_await async_sync_state();
}

void HatchHandler::handle_command(const mgmt::home_assistant::mqttc::CoverCommand& cmd) const
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("HatchHandler::{}", __FUNCTION__);

  auto& hatch = mgmt::device::inventory<mgmt::device::Hatch_t>.get(device_id_);

  switch (cmd) {
  case mgmt::home_assistant::mqttc::CoverCommand::Open:
    hatch.open();
    break;
  case mgmt::home_assistant::mqttc::CoverCommand::Close:
    hatch.close();
    break;
  case mgmt::home_assistant::mqttc::CoverCommand::Stop:
    spdlog::error("Command not supported");// TODO(pp): Error just for the purpose of tests
    break;
  }
}

void HatchHandler::on_error(const mgmt::home_assistant::mqttc::EntityError& error)// NOLINT(readability-convert-member-functions-to-static)
{
  spdlog::error("HatchHandler::{}, message: {}", __FUNCTION__, error.message());
}
}// namespace mgmt::home_assistant::device
