////
//// Created by pp on 7/24/22.
////
//
#include <home_assistant/event/main_board_event_handler.hpp>
#include <home_assistant/event/logger.hpp>

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

  {
    auto device = co_await device::MainBoard::async_create(
            event.device_id,
            factory_,
            device_identity_provider_
    );

    if (!device) {
        throw std::runtime_error{"Cannot create MainBoard device"};
    }

    device_ = std::move(device);
  }

  co_await device_->async_run();
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceRemoved_t& /* event */)
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);

//  assert_has_value(main_board_);

  co_return;
}

boost::asio::awaitable<void> MainBoardEventHandler::operator()(const DeviceStateChanged_t& /* event */)
{
  common::logger::get(Logger)->trace("MainBoardEventHandler::{}", __FUNCTION__);

//  assert_has_value(main_board_);
//
//  co_await main_board_->async_sync_state();// NOLINT(bugprone-unchecked-optional-access)
    co_return; //TODO(bielpa): Remove once implemented
}
} // namespace mgmt::home_assistant::event
