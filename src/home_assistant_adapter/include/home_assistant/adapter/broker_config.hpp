#pragma once

#include <string>

namespace mgmt::home_assistant::adapter
{
    struct BrokerConfig
    {
        std::string username {};
        std::string password {};
        std::string host {};
        std::string port {};
        std::uint16_t keep_alive_interval{ 30 };
        int max_reconnect_attempts{ 5 };
        std::chrono::seconds reconnect_delay{ std::chrono::seconds(5) };
    };
}
