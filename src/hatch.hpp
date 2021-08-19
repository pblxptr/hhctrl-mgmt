#pragma once

#include <optional>

namespace hhctrl::hw {
  enum class HatchStatus {
    Open,
    Closed,
    ChangingPosition,
    Faulty,
    Undefined
  };
  
  class HatchDriver
  {
  public:
    virtual void open() = 0;
    virtual void close() = 0;
    virtual HatchStatus status() const;
  };
}

