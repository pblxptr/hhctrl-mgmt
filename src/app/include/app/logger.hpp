#pragma once

#include <string_view>

namespace mgmt::app {
constexpr auto Logger = "mgmt:app";

void logger_init(std::string_view log_dir);

}// namespace mgmt::app

