#pragma once

#include <home_assistant/adapter/indicator_handler.hpp>
#include <home_assistant/entity_factory_legacy.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <device/indicator.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>
#include <main_board/device/main_board.hpp>

namespace mgmt::home_assistant::device
{
    namespace detail {
        class MainBoardIndicator : public adapter::IndicatorHandler<MainBoardIndicator>
        {
            using Base_t = adapter::IndicatorHandler<MainBoardIndicator>;
            friend adapter::IndicatorHandler<MainBoardIndicator>;

            MainBoardIndicator(
                mgmt::device::DeviceId_t device_id,
                mgmt::device::IndicatorType indicator,
                DeviceIdentity device_identity,
                adapter::BinarySensor_t binary_sensor
            )
                : adapter::IndicatorHandler<MainBoardIndicator>(std::move(binary_sensor))
                , device_id_{std::move(device_id)}
                , indicator_{indicator}
                , identity_{std::move(device_identity)}
            {

            }
        public:
            static boost::asio::awaitable<std::optional<MainBoardIndicator>> async_create(
                mgmt::device::DeviceId_t deviceId,
                mgmt::device::IndicatorType indicator,
                const adapter::EntityFactory& factory,
                const DeviceIdentity& identity)
            {
                const mgmt::device::Device auto& device = mgmt::device::get_device<mgmt::device::MainBoard>(deviceId);
                if (device.indicator_state(indicator) == mgmt::device::IndicatorState::NotAvailable) {
                    co_return std::nullopt;
                }

                auto unique_id = fmt::format("{}_i_{}", adapter::get_unique_id(deviceId, identity),
                                             static_cast<std::underlying_type_t<mgmt::device::IndicatorType>>(indicator));

                auto binary_sensor = factory.create_binary_sensor(std::move(unique_id));
                auto self = MainBoardIndicator{deviceId, indicator, identity, std::move(binary_sensor)};

                if (!(co_await self.async_init())) {
                    co_return std::nullopt;
                }

                co_return self;
            }

        private:
            v2::EntityConfig config() const
            {
                auto config = v2::EntityConfig{};
                config.set("name", fmt::format("{} Indicator", to_string(indicator_)));
                config.set("device", v2::helper::entity_config_basic_device(identity_));

                return config;
            }

            v2::BinarySensorState state() const
            {
                const mgmt::device::Device auto& device = mgmt::device::get_device<mgmt::device::MainBoard>(device_id_);
                const auto state = device.indicator_state(indicator_);

                switch (state) {
                    case mgmt::device::IndicatorState::On:
                        return v2::BinarySensorState::On;
                    case mgmt::device::IndicatorState::Off:
                        return v2::BinarySensorState::Off;
                    default:
                        return v2::BinarySensorState::Off;
                }
            }

        private:
            mgmt::device::DeviceId_t device_id_;
            mgmt::device::IndicatorType indicator_;
            DeviceIdentity identity_;
        };
    } // namespace detail

    class MainBoard
    {
        using IndicatorMap_t = std::unordered_map<mgmt::device::IndicatorType, detail::MainBoardIndicator>;

    MainBoard(IndicatorMap_t indicators);
    public:
        static boost::asio::awaitable<std::optional<MainBoard>> async_create(
            mgmt::device::DeviceId_t device_id,
            const adapter::EntityFactory& factory,
            const DeviceIdentityProvider& identity_provider
        );

        boost::asio::awaitable<void> async_run();
        boost::asio::awaitable<void> async_sync_state();
    private:
        IndicatorMap_t indicators_;
    };
}
