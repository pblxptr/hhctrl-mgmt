#pragma once

#include <string>

namespace mgmt::device
{
  enum class IndicatorState { NotAvailable, Off, On, Blinking };

  inline std::string to_string(const IndicatorState& state)
  {
    using enum IndicatorState;

    switch (state) {
      case NotAvailable:
        return "NotAvailable";
      case Off:
        return "Off";
      case On:
        return "On";
      case Blinking:
        return "Blinking";
    }
    return "";
  }
}