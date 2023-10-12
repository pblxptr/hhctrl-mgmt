#pragma once

#include <home_assistant/adapter/entity_adapter.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/mqtt/sensor.hpp>

namespace mgmt::home_assistant::device
{
    class TempSensor : public adapter::EntityAdapter<adapter::Sensor_t, TempSensor>
    {
    public:
        // movable
        TempSensor(TempSensor&& rhs) noexcept = default;
        TempSensor& operator=(TempSensor&& rhs) noexcept = default;
        // non-copyable
        TempSensor(const TempSensor&) = delete;
        TempSensor& operator=(const TempSensor&) = delete;

        ~TempSensor() = default;

        static boost::asio::awaitable<std::optional<TempSensor>> async_create(
                mgmt::device::DeviceId_t device_id,
                const adapter::DeviceIdentityProvider& identity_provider,
                const adapter::EntityFactory& factory
        );
        mgmt::device::DeviceId_t hardware_id() const;
        mqtt::EntityConfig config() const;
        mqtt::SensorState state() const;

    private:
        TempSensor(mgmt::device::DeviceId_t device_id, adapter::Sensor_t sensor, mqtt::DeviceIdentity device_identity);

    private:
        mgmt::device::DeviceId_t device_id_;
        mqtt::DeviceIdentity device_identity_;
    };
}
