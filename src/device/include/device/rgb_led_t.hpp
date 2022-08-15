//
// Created by pp on 7/24/22.
//

#pragma once

#include "rgb_led.hpp"
#include <main_board/device/sysfs_rgbled.hpp>

namespace mgmt::device {
using RGBLed_t = SysfsRGBLed;
}