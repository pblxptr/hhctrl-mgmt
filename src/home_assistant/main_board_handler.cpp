//
// Created by pp on 7/24/22.
//

#include <main_board/device/main_board.hpp>
#include <device/device_register.hpp>
#include <home_assistant/device/main_board_handler.hpp>
#include <home_assistant/unique_id.hpp>
#include <coro/async_wait.hpp>

/**
 * 000asd_binary_sensor_1
 * 000asd_binary_sensor_2
 */

namespace {
auto create_indicators(
  const mgmt::device::DeviceId_t& device_id,
  const mgmt::home_assistant::EntityFactory& factory,
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider)
{
  auto indicators = std::unordered_map<
    mgmt::device::IndicatorType,
    mgmt::home_assistant::mqttc::BinarySensor_t>{};

  const auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(device_id);
  const auto& device_identity = identity_provider.identity(device_id);

  auto try_create_indicator_entity = [&](const auto& type) {
    if (board.indicator_state(type) == mgmt::device::IndicatorState::NotAvailable) {
      return;
    }
    static int indicator_number = 1;
    const auto indicator_unique_id = fmt::format("{}_i_{}",
      get_unique_id(device_id, device_identity),
      indicator_number++);
    indicators.emplace(type, factory.create_binary_sensor(indicator_unique_id));
  };

  try_create_indicator_entity(mgmt::device::IndicatorType::Status);
  try_create_indicator_entity(mgmt::device::IndicatorType::Warning);
  try_create_indicator_entity(mgmt::device::IndicatorType::Fault);
  try_create_indicator_entity(mgmt::device::IndicatorType::Maintenance);

  return indicators;
}
auto create_restart_button(
  const mgmt::device::DeviceId_t& device_id,
  const mgmt::home_assistant::EntityFactory& factory,
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider)
{
  const auto& device_identity = identity_provider.identity(device_id);
  const auto button_unique_id = fmt::format("{}_restart_btn", get_unique_id(device_id, device_identity));

  return factory.create_button(button_unique_id);
}
}// namespace

namespace mgmt::home_assistant::device {
MainBoardHandler::MainBoardHandler(
  mgmt::device::DeviceId_t device_id,
  const mgmt::home_assistant::DeviceIdentityProvider& identity_provider,
  const mgmt::home_assistant::EntityFactory& factory)
  : device_id_{ std::move(device_id) }
  , identity_provider_{ identity_provider }
  , indicators_{ create_indicators(device_id, factory, identity_provider) }
  , restart_button_{ create_restart_button(device_id, factory, identity_provider) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::{}, device id: {}", __FUNCTION__, device_id_);

  setup();
}

MainBoardHandler::MainBoardHandler(MainBoardHandler&& rhs) noexcept
  : device_id_{ std::move(rhs.device_id_) }
  , identity_provider_{ rhs.identity_provider_ }
  , indicators_{ std::move(rhs.indicators_) }
  , restart_button_{ std::move(rhs.restart_button_) }
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::(MainBoardHandler&&)");

  setup();
}

MainBoardHandler& MainBoardHandler::operator=(MainBoardHandler&& rhs) noexcept
{
  device_id_ = std::move(rhs.device_id_);
  indicators_ = std::move(rhs.indicators_);
  restart_button_ = std::move(rhs.restart_button_);

  setup();

  return *this;
}

mgmt::device::DeviceId_t MainBoardHandler::hardware_id() const
{
  return device_id_;
}

boost::asio::awaitable<void> MainBoardHandler::async_connect()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::{}", __FUNCTION__);

  // Connect indicators
  for (auto&& [_, indicator] : indicators_) {
    co_await indicator.async_connect();
  }
  // Connect restart button
  co_await restart_button_.async_connect();
}

boost::asio::awaitable<void> MainBoardHandler::async_sync_state()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::{}", __FUNCTION__);

  const auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);

  // Sync indicators
  for (auto&& [type, indicator] : indicators_) {
    auto binary_sensor_state = mgmt::home_assistant::mqttc::BinarySensorState{};
    const auto current_state = board.indicator_state(type);

    switch (current_state) {
    case mgmt::device::IndicatorState::NotAvailable:
    case mgmt::device::IndicatorState::Off:
      binary_sensor_state = mgmt::home_assistant::mqttc::BinarySensorState::Off;
      break;
    case mgmt::device::IndicatorState::On:
    case mgmt::device::IndicatorState::Blinking:
      binary_sensor_state = mgmt::home_assistant::mqttc::BinarySensorState::On;
      break;
    }
    co_await indicator.async_set_state(binary_sensor_state);
  }
}

void MainBoardHandler::setup()
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::{}", __FUNCTION__);

  // Setup indicators
  for (auto&& [type, indicator] : indicators_) {
    /*
     * type = type it has to be formed that way, because according to the standard
     * structure binding does not introduce variable name. Compiles with gcc but not with clang
     * https://stackoverflow.com/questions/46114214/lambda-implicit-capture-fails-with-variable-declared-from-structured-binding
     */
    indicator.set_ack_handler([this, type = type]() -> boost::asio::awaitable<void> { co_await async_set_config_indicator(type); });
    indicator.set_error_handler([this](const auto& ec) { on_error(ec); });
  }
  // Setup restart button
  restart_button_.set_ack_handler([this]() -> boost::asio::awaitable<void> { co_await async_set_config_restart_button(); });
  restart_button_.set_error_handler([this](const auto& ec) { on_error(ec); });
  restart_button_.on_command([this](auto&&) {
    auto& board = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);
    board.restart();
  });
}

boost::asio::awaitable<void> MainBoardHandler::async_set_config_indicator(const mgmt::device::IndicatorType& type)
{
  using std::to_string;

  auto& indicator = indicators_.at(type);
  auto config = mgmt::home_assistant::mqttc::EntityConfig{ indicator.unique_id() };
  config.set("name", fmt::format("{} Indicator", to_string(type)));
  config.set("device_class", "light");
  config.set("entity_category", "diagnostic");
  config.set("device", mqttc::helper::entity_config_basic_device(identity_provider_.identity(device_id_)));
  co_await indicator.async_set_config(std::move(config));

  // Wait until entity is configured on remote
  co_await common::coro::async_wait(std::chrono::seconds(1));

  co_await indicator.async_set_availability(mgmt::home_assistant::mqttc::Availability::Online);
  co_await async_sync_state();
}

boost::asio::awaitable<void> MainBoardHandler::async_set_config_restart_button()
{
  auto config = mgmt::home_assistant::mqttc::EntityConfig{ restart_button_.unique_id() };
  config.set("name", "Restart board");
  config.set("device_class", "restart");
  config.set("device", mqttc::helper::entity_config_basic_device(identity_provider_.identity(device_id_)));
  co_await restart_button_.async_set_config(std::move(config));

  // Wait until entity is configured on remote
  co_await common::coro::async_wait(std::chrono::seconds(1));

  co_await restart_button_.async_set_availability(mgmt::home_assistant::mqttc::Availability::Online);
}

void MainBoardHandler::on_error(const mgmt::home_assistant::mqttc::EntityError& /* error */)
{
  common::logger::get(mgmt::home_assistant::Logger)->debug("MainBoardHandler::{}", __FUNCTION__);
}
}// namespace mgmt::home_assistant::device
