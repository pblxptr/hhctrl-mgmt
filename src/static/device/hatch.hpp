#pragma once

#include <concepts>

namespace mgmt::device
{
  enum class HatchStatus { Undefined, Open, Closed, ChangingPosition, Faulty };

  template<class T>
  concept Hatch = requires(T t)
  {
    t.open();
    t.close();
    { t.status() } -> std::same_as<HatchStatus>;
  };
}