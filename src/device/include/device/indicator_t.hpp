#pragma once

#include "indicator.hpp"
#include "led_indicator.hpp"
#include "rgb_indicator.hpp"

namespace mgmt::device {
template<Indicator... SupportedTypes>
class IndicatorProxy
{
  using IndicatorVariant_t = std::variant<SupportedTypes...>;

public:
  explicit IndicatorProxy(IndicatorVariant_t indicator)
    : indicator_{ std::move(indicator) }
  {}
  // movable
  IndicatorProxy(IndicatorProxy&&) noexcept = default;
  IndicatorProxy& operator=(IndicatorProxy&&) noexcept = default;
  // non-copyable
  IndicatorProxy(const IndicatorProxy&) = delete;
  IndicatorProxy& operator=(const IndicatorProxy&) = delete;

  ~IndicatorProxy() = default;

  IndicatorType type() const
  {
    return std::visit([](auto&& indicator) { return indicator.type(); }, indicator_);
  }

  IndicatorState state() const
  {
    return std::visit([](auto&& indicator) { return indicator.state(); }, indicator_);
  }
  void set_state(IndicatorState state)
  {
    std::visit([state](auto&& indicator) { indicator.set_state(state); }, indicator_);
  }

private:
  IndicatorVariant_t indicator_;
};

using Indicator_t = IndicatorProxy<
  LedIndicator,
  RGBIndicator>;
}// namespace mgmt::device
