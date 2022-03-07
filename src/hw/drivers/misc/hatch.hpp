#pragma once

#include <optional>
#include <hw/drivers/driver.hpp>

namespace hw::drivers {
  enum class HatchStatus {
    Open,
    Closed,
    ChangingPosition,
    Faulty,
    Undefined
  };
  class HatchDriver : public Driver
  {
  public:
    virtual void open() const = 0;
    virtual void close() const = 0;
    virtual HatchStatus status() const = 0;
  };
}

