#pragma once

#include <exception>

namespace common::coro {
inline void rethrow(std::exception_ptr eptr)
{
  if (eptr) {
    std::rethrow_exception(eptr);
  }
}
}// namespace common::coro