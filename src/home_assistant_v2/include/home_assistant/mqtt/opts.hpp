#pragma once

#include <async_mqtt/all.hpp>

namespace mgmt::home_assistant::mqtt
{
    using Pubopts_t = async_mqtt::pub::opts;
    using Retain_t = async_mqtt::pub::retain;
    using Qos_t = async_mqtt::qos;
}
