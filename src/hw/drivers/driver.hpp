#pragma once

namespace hw::drivers
{
  class Driver
  {
  public:
    Driver() = default;
    virtual ~Driver() = default;
    Driver(const Driver&) = delete;
    Driver(Driver&&) = default;
    Driver& operator=(const Driver&) = delete;
    Driver& operator=(Driver&&) = default;
  };
}