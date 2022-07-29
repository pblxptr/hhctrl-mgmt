#pragma once

namespace common::data {
enum class IndicatorType { Status,
  Warning,
  Maintenance,
  Fault };
enum class IndicatorState { SteadyOn,
  SteadyOff };
}// namespace common::data
