#pragma once

#include <home_assistant/adapter/entity_adapter.hpp>
//#include <home_assistant/adapter/indicator_handler.hpp>
#include <home_assistant/mqtt/binary_sensor2.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <device/indicator.hpp>
#include <device/device_id.hpp>
#include <device/device_register.hpp>
#include <main_board/device/main_board.hpp>

namespace mgmt::home_assistant::device
{
namespace detail {
    class Indicator : public adapter::EntityAdapter<adapter::BinarySensor_t, Indicator>
    {
        Indicator(
            mgmt::device::DeviceId_t device_id,
            mgmt::device::IndicatorType indicator,
            v2::DeviceIdentity device_identity,
            adapter::BinarySensor_t binary_sensor
        );
    public:
        static boost::asio::awaitable<std::optional<Indicator>> async_create(
            mgmt::device::DeviceId_t device_id,
            mgmt::device::IndicatorType indicator_type,
            const adapter::EntityFactory& factory,
            const v2::DeviceIdentity& identity
        );
        v2::EntityConfig config() const;
        v2::BinarySensorState state() const;
    private:
        mgmt::device::DeviceId_t device_id_;
        mgmt::device::IndicatorType indicator_type_;
        v2::DeviceIdentity identity_;
    };

    class RestartButton : public adapter::EntityAdapter<adapter::Button_t, RestartButton>
    {
        RestartButton(
            mgmt::device::DeviceId_t device_id,
            v2::DeviceIdentity device_identity,
            adapter::Button_t button
        );
        public:
            static boost::asio::awaitable<std::optional<RestartButton>> async_create(
                mgmt::device::DeviceId_t device_id,
                adapter::EntityFactory& factory,
                const v2::DeviceIdentity& identity
            );

            v2::EntityConfig config() const;
            boost::asio::awaitable<void> async_handle_recv_value(const v2::ButtonCommand& command);
        private:
            mgmt::device::DeviceId_t device_id_;
            v2::DeviceIdentity identity_;
        };
} // namespace detail

    class MainBoard
    {
        using IndicatorMap_t = std::unordered_map<mgmt::device::IndicatorType, detail::Indicator>;

        MainBoard(IndicatorMap_t indicators, detail::RestartButton restart_button);
    public:
        static boost::asio::awaitable<std::optional<MainBoard>> async_create(
            mgmt::device::DeviceId_t device_id,
            const adapter::DeviceIdentityProvider& identity_provider,
            adapter::EntityFactory& factory
        );
        boost::asio::awaitable<void> async_run();
        boost::asio::awaitable<void> async_sync_state();
    private:
        IndicatorMap_t indicators_;
        detail::RestartButton restart_button_;
    };
}
