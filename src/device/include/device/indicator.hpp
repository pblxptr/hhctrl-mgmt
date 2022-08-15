#pragma once

#include <string>
#include <variant>
#include "device.hpp"

namespace mgmt::device {
enum class IndicatorState { NotAvailable,
  Off,
  On,
  Blinking };

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

enum class IndicatorType { Status,
  Warning,
  Fault,
  Maintenance };

inline std::string to_string(const IndicatorType& type)
{
  using enum IndicatorType;

  switch (type) {
  case Status:
    return "Status";
  case Warning:
    return "Warning";
  case Fault:
    return "Fault";
  case Maintenance:
    return "Maintenance";
  }
  return "";
}

template<class T>
concept Indicator = Device<T> and requires(T v)
{
  {
    v.type()
    } -> std::same_as<IndicatorType>;
  {
    v.state()
    } -> std::same_as<IndicatorState>;
  v.set_state(std::declval<IndicatorState>());
};
}// namespace mgmt::device