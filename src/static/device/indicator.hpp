#pragma once

#include <string>
#include <variant>

#include <static/device/led_indicator.hpp>
#include <static/device/rgb_indicator.hpp>

namespace mgmt::device
{
  using Indicator_t = std::variant<
    mgmt::device::LedIndicator,
    mgmt::device::RGBIndicator
  >;
}