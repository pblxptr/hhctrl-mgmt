//
// Created by bielpa on 27.12.23.
//

#include <home_assistant/mqtt/error.hpp>

namespace {
    static const auto ErrorCategoryInstance = mgmt::home_assistant::mqtt::detail::ErrorCategory{};

    namespace mqtt = mgmt::home_assistant::mqtt;
}

namespace mgmt::home_assistant::mqtt {
    std::error_code make_error_code(mqtt::ErrorCode error)
    {
        return {static_cast<int>(error), ErrorCategoryInstance};
    }
}
