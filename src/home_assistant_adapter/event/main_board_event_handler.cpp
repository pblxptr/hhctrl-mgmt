////
//// Created by pp on 7/24/22.
////
//
#include <home_assistant/event/main_board_event_handler.hpp>
#include <home_assistant/event/logger.hpp>
#include <coro/co_spawn.hpp>

namespace {
using mgmt::home_assistant::device::MainBoard;

//void assert_has_value(const std::optional<MainBoard>& main_board)
//{
//  if (not main_board.has_value()) {
//    throw std::runtime_error("std::optional<MainBoardHandler> does not have a value");
//  }
//}
}// namespace

namespace mgmt::home_assistant::event {
MainBoardEventHandler::MainBoardEventHandler(
  const adapter::EntityFactory& factory,
  const DeviceIdentityProvider& device_identity_provider)
  : factory_{ factory }
  , device_identity_provider_{ device_identity_provider }
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceCreated_t& event)
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);

  if (device_) {
    throw std::runtime_error("Unexpected value in std::optional<MainBoardHandler>");
  }

  auto device = co_await device::MainBoard::async_create(
            event.device_id,
            device_identity_provider_,
            factory_
  );

   if (!device) {
        throw std::runtime_error{"Cannot create MainBoard device"};
   }

    device_.emplace(std::move(*device));

  auto executor = co_await boost::asio::this_coro::executor;
  boost::asio::co_spawn(executor, device_->async_run(), common::coro::rethrow);
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceRemoved_t& /* event */)
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);

//  assert_has_value(device_);

  co_return;
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceStateChanged_t& /* event */)
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);

//  assert_has_value(device_);

  if (!device_) {
      co_return;
  }

  co_await device_->async_sync_state();// NOLINT(bugprone-unchecked-optional-access)
}
} // namespace mgmt::home_assistant::event
