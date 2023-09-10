#pragma once

#include <home_assistant/adapter/entity_adapter.hpp>
#include <home_assistant/adapter/unique_id.hpp>
#include <home_assistant/adapter/logger.hpp>
#include <home_assistant/device_identity_provider.hpp>
#include <home_assistant/entity_factory.hpp>
#include <home_assistant/mqtt/coverv2.hpp>
#include <device/device_register.hpp>

namespace mgmt::home_assistant::device
{
    class Hatch : public adapter::EntityAdapter<adapter::Cover_t, Hatch>
    {
    public:
        // movable
        Hatch(Hatch&& rhs) noexcept = default;
        Hatch& operator=(Hatch&& rhs) noexcept = default;
        // non-copyable
        Hatch(const Hatch&) = delete;
        Hatch& operator=(const Hatch&) = delete;

        ~Hatch() = default;

        static boost::asio::awaitable<std::optional<Hatch>> async_create(
                mgmt::device::DeviceId_t device_id,
                const DeviceIdentityProvider& identity_provider,
                const adapter::EntityFactory& factory
        );

        const mgmt::device::DeviceId_t& hardware_id() const;
        v2::EntityConfig config() const;
        v2::CoverState state() const;
        boost::asio::awaitable<void> async_handle_recv_value(const v2::CoverCommand& command);
    private:
        Hatch(mgmt::device::DeviceId_t device_id, DeviceIdentity device_identity, adapter::Cover_t cover);
    private:
        mgmt::device::DeviceId_t device_id_;
        DeviceIdentity device_identity_;
    };
}
