#pragma once

#include <atomic>
#include <fmt/format.h>

namespace mgmt::device
{
  class IdGenerator
  {
  public:
    static auto new_id()
    {
      return fmt::format("dev-{}", id_number_++);
    }
  private:
    static inline std::atomic<size_t> id_number_;
  };
}