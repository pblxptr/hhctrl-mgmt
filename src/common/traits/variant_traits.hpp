#pragma once

#include <variant>
#include <concepts>

namespace common::traits {
template<class Variant, class T>
concept VariantContains = requires(Variant v, T new_value)
{
  // Try to assign new value to variant.
  // If it fails, then it means that variant does not hold this particular type.
  std::holds_alternative<T>(v) == true;
};
}// namespace common::traits