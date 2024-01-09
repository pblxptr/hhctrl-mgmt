//
// Created by bielpa on 05.10.23.
//

#include <home_assistant/device/temp_sensor.hpp>
#include <device/temp_sensor_t.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::device
{
    TempSensor::TempSensor(mgmt::device::DeviceId_t device_id, adapter::Sensor_t sensor, mqtt::DeviceIdentity device_identity)
        : EntityAdapter<adapter::Sensor_t, TempSensor>{std::move(sensor)}
        , device_id_{std::move(device_id)}
        , device_identity_{std::move(device_identity)}
    {}

    boost::asio::awaitable<std::optional<TempSensor>> TempSensor::async_create(
            mgmt::device::DeviceId_t device_id,
            const adapter::DeviceIdentityProvider& identity_provider,
            const adapter::EntityFactory& factory
    )
    {
        auto cover = factory.create_sensor(adapter::get_unique_id(device_id, identity_provider.identity(device_id)));
        auto handler = TempSensor{device_id, std::move(cover), identity_provider.identity(device_id)};

        const auto initialized = co_await handler.async_init();
        if (!initialized) {
            co_return std::nullopt;
        }

        co_return handler;
    }

    mgmt::device::DeviceId_t TempSensor::hardware_id() const
    {
        return device_id_;
    }

    mqtt::EntityConfig TempSensor::config() const
    {
        auto config = mqtt::EntityConfig{};
        config.set("name", "Temp sensor");
        config.set("device_class", "temperature");
        config.set("unit_of_measurement", "°C");
        config.set("device", mqtt::helper::entity_config_basic_device(device_identity_));

        return config;
    }

    mqtt::SensorState TempSensor::state() const
    {
        const auto& temp_sensor = mgmt::device::get_device<mgmt::device::TempSensor_t>(device_id_);

        return fmt::format("{:.1f}", temp_sensor.value());
    }
} // namespace mgmt::home_assistant::device
