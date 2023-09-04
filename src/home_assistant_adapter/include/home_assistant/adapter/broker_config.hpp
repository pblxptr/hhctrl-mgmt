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
    };
}
