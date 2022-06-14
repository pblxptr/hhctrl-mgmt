#pragma once

#include <string>

namespace mgmt::device
{
  enum class IndicatorType { Status, Warning, Fault, Maintenance };

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
}