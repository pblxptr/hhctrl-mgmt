//
// Created by pp on 7/24/22.
//

#include <home_assistant/device/main_board_event_handler.hpp>
#include <home_assistant/logger.hpp>

namespace mgmt::home_assistant::device {
MainBoardEventHandler::MainBoardEventHandler(
  const EntityFactory& factory,
  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }
  , device_identity_provider_{ device_identity_provider }
{
  spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceCreated_t& event)
{
  spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
  assert(main_board_ == std::nullopt);

  main_board_.emplace(event.device_id, device_identity_provider_, factory_);
  co_await main_board_->async_connect();
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceRemoved_t& /* event */)
{
  spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
  assert(main_board_ != std::nullopt);

  co_return;
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceStateChanged_t& /* event */)
{
  spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
  assert(main_board_ != std::nullopt);

  co_await main_board_->async_sync_state();

  co_return;
}
void MainBoardEventHandler::on_error()
{
  spdlog::debug("MainBoardEventHandler::{}", __FUNCTION__);
}
}// namespace mgmt::home_assistant::device