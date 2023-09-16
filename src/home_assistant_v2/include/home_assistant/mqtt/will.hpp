#pragma once

#include <home_assistant/mqtt/opts.hpp>
#include <string>

namespace mgmt::home_assistant::mqtt {
    struct WillConfig {
        std::string topic{};
        std::string message{};
        Pubopts_t pubopts{};
    };
}
