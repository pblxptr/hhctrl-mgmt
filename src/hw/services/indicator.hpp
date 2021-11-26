#pragma once

namespace hw::services
{
  enum class IndicatorType { Status, Warning, Maintenance, Fault };
  enum class IndicatorState { SteadyOn, SteadyOff };
}