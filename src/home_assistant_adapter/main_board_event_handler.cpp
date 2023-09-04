////
//// Created by pp on 7/24/22.
////
//
//#include <home_assistant/device/main_board_event_handler.hpp>
//#include <home_assistant/logger.hpp>
//
//namespace {
//using mgmt::home_assistant::device::MainBoardHandler;
//
//void assert_has_value(const std::optional<MainBoardHandler>& main_board)
//{
//  if (not main_board.has_value()) {
//    throw std::runtime_error("std::optional<MainBoardHandler> does not have a value");
//  }
//}
//}// namespace
//
//namespace mgmt::home_assistant::device {
//MainBoardEventHandler::MainBoardEventHandler(
//  const EntityFactory& factory,
//  const mgmt::home_assistant::DeviceIdentityProvider& device_identity_provider)
//  : factory_{ factory }
//  , device_identity_provider_{ device_identity_provider }
//{
//  common::logger::get(mgmt::home_assistant::Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);
//}
//
//boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceCreated_t& event)
//{
//  common::logger::get(mgmt::home_assistant::Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);
//
//  if (main_board_) {
//    throw std::runtime_error("Unexpected value in std::optional<MainBoardHandler>");
//  }
//
//
//  main_board_.emplace(event.device_id, device_identity_provider_, factory_);
//  co_await main_board_->async_connect();
//}
//
//boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceRemoved_t& /* event */)
//{
//  common::logger::get(mgmt::home_assistant::Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);
//
//  assert_has_value(main_board_);
//
//  co_return;
//}
//
//boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceStateChanged_t& /* event */)
//{
//  common::logger::get(mgmt::home_assistant::Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);
//
//  assert_has_value(main_board_);
//
//  co_await main_board_->async_sync_state();// NOLINT(bugprone-unchecked-optional-access)
//}
//}// namespace mgmt::home_assistant::device
