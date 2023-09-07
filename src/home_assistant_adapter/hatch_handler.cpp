////
//// Created by pp on 7/24/22.
////
//
//#include <home_assistant/adapter/hatch_handler.hpp>
//#include <home_assistant/adapter/logger.hpp>
//#include <home_assistant/device_identity_provider.hpp>
//#include <device/hatch_t.hpp>
//#include <coro/async_wait.hpp>
//
//namespace mgmt::home_assistant::adapter {
//HatchHandler::HatchHandler(mgmt::device::DeviceId_t device_id, Cover_t cover, DeviceIdentity device_identity)
//    : device_id_{std::move(device_id)}
//    , cover_{std::move(cover)}
//    , device_identity_{std::move(device_identity)}
//{
//  common::logger::get(mgmt::home_assistant::adapter::Logger)->debug("HatchHandler::{}, device id: {}", __FUNCTION__, device_id_);
//
////  cover_.on_reconnected([this]() -> boost::asio::awaitable<void> {
////      co_await async_configure();
////  });
//}
//
//boost::asio::awaitable<std::optional<HatchHandler>> HatchHandler::async_create(
//    mgmt::device::DeviceId_t device_id,
//    const DeviceIdentityProvider& identity_provider,
//    const EntityFactory& factory)
//{
//    auto cover = factory.create_cover(get_unique_id(device_id, identity_provider.identity(device_id)));
//    auto handler = HatchHandler{device_id, std::move(cover), identity_provider.identity(device_id)};
//
//    const auto connected = co_await handler.async_connect();
//    if (!connected) {
//        co_return std::nullopt;
//    }
//
//    const auto configured = co_await handler.async_configure();
//    if (!configured) {
//        co_return std::nullopt;
//    }
//
//    co_return handler;
//}
//
//mgmt::device::DeviceId_t HatchHandler::hardware_id() const
//{
//  return device_id_;
//}
//
//boost::asio::awaitable<void> HatchHandler::async_run()
//{
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//
//    while (true) {
//        const auto command = co_await cover_.async_receive();
//
//        if (command) {
//            co_await async_handle_command(command.value());
//        }
//        else {
//            bool recovered = false;
//
//            if (command.error().code() == v2::ErrorCode::Disconnected) {
//                recovered = co_await async_handle_disconnected_error();
//            }
//            else if (command.error().code() == v2::ErrorCode::Reconnected) {
//                recovered = co_await async_handle_reconnected_error();
//            }
//            else {
//                common::logger::get(mgmt::home_assistant::adapter::Logger)->error("HatchHandler unhandled error: '{}'", command.error().what());
//            }
//
//            if (!recovered) {
//                common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler could not recover from error: '{}'", command.error().what());
//            }
//        }
//    }
//}
//
//boost::asio::awaitable<void> HatchHandler::async_sync_state()
//{
//  common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//
//  const auto& hatch = mgmt::device::get_device<mgmt::device::Hatch_t>(device_id_);
//  auto cover_state = mgmt::home_assistant::v2::CoverState{};
//
//  switch (hatch.status()) {
//  case mgmt::device::HatchState::Open:
//    cover_state = mgmt::home_assistant::v2::CoverState::Open;
//    break;
//  case mgmt::device::HatchState::Closed:
//    cover_state = mgmt::home_assistant::v2::CoverState::Closed;
//    break;
//  default:
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->error("Unsupported state change for cover entity with unique id: '{}'", cover_.unique_id());
//    co_return;
//  }
//
//  auto error = co_await cover_.async_set_state(cover_state); //TODO(bielpa): Add retain
//  if (error) {
//      common::logger::get(mgmt::home_assistant::adapter::Logger)->error("Cannot sync state for a cover with unique id: '{}', error: '{}'", cover_.unique_id(), error.what());
//  }
//}
//
//boost::asio::awaitable<bool> HatchHandler::async_connect()
//{
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//
//    const auto error = co_await cover_.async_connect();
//    if (error) {
//        common::logger::get(mgmt::home_assistant::adapter::Logger)->error("Cannot establish connection for cover entity with unique id: '{}'", cover_.unique_id());
//        co_return false;
//    }
//
//    co_return true;
//}
//
//boost::asio::awaitable<bool> HatchHandler::async_configure()
//{
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//    // Configure
//    {
//        auto config = mgmt::home_assistant::v2::EntityConfig{};
//        config.set("name", "Cover");
//        config.set("device_class", "door");
//        config.set("device", v2::helper::entity_config_basic_device(device_identity_));
//        config.set(v2::CoverConfig::Property::PayloadStop, nullptr);// Stop command not implemented
//
//        const auto error = co_await cover_.async_configure();
//
//        if (error) {
//            common::logger::get(mgmt::home_assistant::adapter::Logger)->error("Cannot configure cover entity with unique id: '{}'", cover_.unique_id());
//            co_return false;
//        }
//    }
//
//    // Wait until entity is configured on remote
//    co_await common::coro::async_wait(std::chrono::seconds(1));
//
//    co_await cover_.async_set_availability(v2::Availability::Online);
//    co_await async_sync_state();
//
//    co_return true;
//}
//
//boost::asio::awaitable<void> HatchHandler::async_handle_command(const v2::CoverCommand& command)
//{
//    if (std::holds_alternative<v2::CoverTiltCommand>(command)) {
//        common::logger::get(mgmt::home_assistant::adapter::Logger)->error("Tilt command is currently not supported");
//        co_return;
//    }
//
//    const auto& cmd = std::get<v2::CoverSwitchCommand>(command);
//
//    auto& hatch = mgmt::device::Inventory<mgmt::device::Hatch_t>.get(device_id_);
//    const auto status = hatch.status();
//
//    switch (cmd) {
//        case mgmt::home_assistant::v2::CoverSwitchCommand::Open:
//            common::logger::get(mgmt::home_assistant::adapter::Logger)->debug("HatchHandler::{}, current: {}, requested: 'open'.", __FUNCTION__, to_string(status));
//            hatch.open();
//            break;
//        case mgmt::home_assistant::v2::CoverSwitchCommand::Close:
//            common::logger::get(mgmt::home_assistant::adapter::Logger)->debug("HatchHandler::{}, current: {}, requested: 'close'.", __FUNCTION__, to_string(status));
//            hatch.close();
//            break;
//        case mgmt::home_assistant::v2::CoverSwitchCommand::Stop:
//            common::logger::get(mgmt::home_assistant::adapter::Logger)->warn("HatchHandler::{}, command not supported.", __FUNCTION__);
//            break;
//    }
//}
//
//
//boost::asio::awaitable<bool> HatchHandler::async_handle_disconnected_error()
//{
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//
//    const auto error_code = co_await async_connect();
//
//    if (error_code) {
//        co_return false;
//    }
//
//    co_return co_await async_configure();
//}
//
//boost::asio::awaitable<bool> HatchHandler::async_handle_reconnected_error()
//{
//    common::logger::get(mgmt::home_assistant::adapter::Logger)->trace("HatchHandler::{}", __FUNCTION__);
//
//    co_return co_await async_configure();
//}
//
//
//}// namespace mgmt::home_assistant::device
//
//#include <home_assistant/adapter/indicator_handler.hpp>
