#include <home_assistant/device/main_board.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <coro/co_spawn.hpp>

//
namespace mgmt::home_assistant::device {
    namespace detail {
        Indicator::Indicator(
            mgmt::device::DeviceId_t device_id,
            mgmt::device::IndicatorType indicator_type,
            mqtt::DeviceIdentity device_identity,
            adapter::BinarySensor_t binary_sensor
        ) : EntityAdapter<adapter::BinarySensor_t, Indicator>{std::move(binary_sensor)}
          , device_id_{std::move(device_id)}
          , indicator_type_{indicator_type}
          , identity_{std::move(device_identity)}
        {}

        boost::asio::awaitable<std::optional<Indicator>> Indicator::async_create(
            mgmt::device::DeviceId_t device_id,
            mgmt::device::IndicatorType indicator_type,
            const adapter::EntityFactory &factory,
            const mqtt::DeviceIdentity &identity
        ) {
            const mgmt::device::Device auto &device = mgmt::device::get_device<mgmt::device::MainBoard>(device_id);
            if (device.indicator_state(indicator_type) == mgmt::device::IndicatorState::NotAvailable) {
                co_return std::nullopt;
            }

            auto unique_id = fmt::format("{}_i_{}", adapter::get_unique_id(device_id, identity),
                                         static_cast<std::underlying_type_t<mgmt::device::IndicatorType>>(indicator_type));

            auto binary_sensor = factory.create_binary_sensor(unique_id);
            auto self = Indicator{device_id, indicator_type, identity, std::move(binary_sensor)};

            if (!(co_await self.async_init())) {
                co_return std::nullopt;
            }

            co_return self;
        }

        mqtt::EntityConfig Indicator::config() const
        {
            auto config = mqtt::EntityConfig{};
            config.set("name", fmt::format("{} Indicator", to_string(indicator_type_)));
            config.set("device", mqtt::helper::entity_config_basic_device(identity_));

            return config;
        }

        mqtt::BinarySensorState Indicator::state() const
        {
            const mgmt::device::Device auto &device = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);
            const auto state = device.indicator_state(indicator_type_);

            switch (state) {
                case mgmt::device::IndicatorState::On:
                    return mqtt::BinarySensorState::On;
                default:
                    return mqtt::BinarySensorState::Off;
            }
        }

        RestartButton::RestartButton(
            mgmt::device::DeviceId_t device_id,
            mqtt::DeviceIdentity device_identity,
            adapter::Button_t button
        ) : EntityAdapter<adapter::Button_t, RestartButton>{std::move(button)}
          , device_id_{std::move(device_id)}
          , identity_{std::move(device_identity)}
        {}
        boost::asio::awaitable<std::optional<RestartButton>> RestartButton::async_create(
            mgmt::device::DeviceId_t device_id,
            adapter::EntityFactory& factory,
            const mqtt::DeviceIdentity& identity
        )
        {
            auto button = factory.create_button(fmt::format("{}_restart_btn", adapter::get_unique_id(device_id, identity)));
            auto self = RestartButton{device_id, identity, std::move(button)};

            const auto initialized = co_await self.async_init();
            if (!initialized) {
                co_return std::nullopt;
            }

            co_return self;
        }

        mqtt::EntityConfig RestartButton::config() const
        {
            auto config = mqtt::EntityConfig{};
            config.set("name", "Restart board");
            config.set("device_class", "restart");
            config.set("device", mqtt::helper::entity_config_basic_device(identity_));

            return config;
        }

        boost::asio::awaitable<void> RestartButton::async_handle_recv_value(const mqtt::ButtonCommand& /* command */)
        {
            common::logger::get(adapter::Logger)->trace("RestartButton::{}", __FUNCTION__);

            mgmt::device::Device auto& device = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);
            device.restart();

            co_return;
        }
    } // namespace detail

    boost::asio::awaitable<std::optional<MainBoard>> MainBoard::async_create(
            mgmt::device::DeviceId_t device_id,
            const adapter::DeviceIdentityProvider& identity_provider,
            adapter::EntityFactory&  factory
    )
    {
        auto identity = identity_provider.identity(device_id);

        // Indicators
        auto fault_indicator = co_await detail::Indicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Fault,
                factory,
                identity
        );
        auto status_indicator = co_await detail::Indicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Status,
                factory,
                identity
        );
        auto warning_indicator = co_await detail::Indicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Warning,
                factory,
                identity
        );
        auto maintenance_indicator = co_await detail::Indicator::async_create(
                device_id,
                mgmt::device::IndicatorType::Maintenance,
                factory,
                identity
        );

        if (!fault_indicator || !status_indicator || !warning_indicator || !maintenance_indicator) {
            co_return std::nullopt;
        }
        auto indicators = IndicatorMap_t {};
        indicators.emplace(mgmt::device::IndicatorType::Fault, std::move(*fault_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Status, std::move(*status_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Warning, std::move(*warning_indicator));
        indicators.emplace(mgmt::device::IndicatorType::Maintenance, std::move(*maintenance_indicator));

        // Buttons
        auto restart_button = co_await detail::RestartButton::async_create(
            device_id,
            factory,
            identity
        );
//
        if (!restart_button) {
            co_return std::nullopt;
        }

        co_return MainBoard{std::move(indicators), std::move(*restart_button)};
    }

    MainBoard::MainBoard(IndicatorMap_t indicators, detail::RestartButton restart_button)
        : indicators_{std::move(indicators)}
        , restart_button_{std::move(restart_button)}
    {}

    boost::asio::awaitable<void> MainBoard::async_run()
    {
        auto executor = co_await boost::asio::this_coro::executor;

        // Spawn coroutines for indicators
        for (auto &&[type, indicator] : indicators_) {
            boost::asio::co_spawn(executor, indicator.async_run(), common::coro::rethrow);
        }

        // Spawn coroutines for buttons
        boost::asio::co_spawn(executor, restart_button_.async_run(), common::coro::rethrow);

        co_return;
    }

    boost::asio::awaitable<void> MainBoard::async_sync_state()
    {
        // Sync indicators
        for (auto&& [k, indicator] : indicators_) {
            co_await indicator.async_sync_state();
        }
    }
}
