#pragma once

#include "indicator.hpp"
#include "led.hpp"

namespace mgmt::device {
class LedIndicator
{
public:
  LedIndicator(IndicatorType type, Led_t led)
    : type_{ type }
    , led_{ std::move(led) }
  {}
  // movable
  LedIndicator(LedIndicator&&) noexcept = default;
  LedIndicator& operator=(LedIndicator&&) noexcept = default;
  // non-copyable
  LedIndicator(const LedIndicator&) = delete;
  LedIndicator& operator=(const LedIndicator&) = delete;

  ~LedIndicator() = default;

  constexpr IndicatorType type() const
  {
    return type_;
  }

  //TODO(pp) Implement, remove nolint
  IndicatorState state() const //NOLINT(readability-convert-member-functions-to-static)
  {
    return IndicatorState::NotAvailable;
  }

  void set_state(IndicatorState state)
  {
    switch (state) {
    case IndicatorState::Off:
      led_.set_brightness(0);
      break;
    case IndicatorState::On:
      led_.set_brightness(255);
      break;
    case IndicatorState::Blinking:
    case IndicatorState::NotAvailable:
      break;
    }
  }

private:
  IndicatorType type_;
  Led_t led_;
};

}// namespace mgmt::device
