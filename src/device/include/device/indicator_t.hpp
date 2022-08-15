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

  IndicatorProxy(const IndicatorProxy&) = delete;
  IndicatorProxy& operator=(const IndicatorProxy&) = delete;
  IndicatorProxy(IndicatorProxy&&) noexcept = default;
  IndicatorProxy& operator=(IndicatorProxy&&) noexcept = default;

  IndicatorType type() const
  {
    return std::visit([](auto&& v) { return v.type(); }, indicator_);
  }

  IndicatorState state() const
  {
    return std::visit([](auto&& v) { return v.state(); }, indicator_);
  }
  void set_state(IndicatorState state)
  {
    std::visit([state](auto&& v) { v.set_state(state); }, indicator_);
  }

private:
  IndicatorVariant_t indicator_;
};

using Indicator_t = IndicatorProxy<
  LedIndicator,
  RGBIndicator>;
}// namespace mgmt::device