#pragma once

#include <memory>

#include <device/indicator.hpp>
#include <device/rgb_led_t.hpp>

namespace mgmt::device {
class RGBIndicator
{
public:
  RGBIndicator(IndicatorType, std::shared_ptr<RGBLed_t>);
  IndicatorType type() const;
  IndicatorState state() const;
  void set_state(IndicatorState);

private:
  IndicatorType type_;
  std::shared_ptr<RGBLed_t> rgbled_;
};
}// namespace mgmt::device