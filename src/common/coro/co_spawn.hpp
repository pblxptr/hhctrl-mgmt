#pragma once

#include <exception>

namespace common::coro {
  void rethrow(std::exception_ptr eptr)
  {
    if (eptr) {
      std::rethrow_exception(eptr);
    }
  }
}