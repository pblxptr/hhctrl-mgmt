#pragma once

namespace common::utils {
  template<class...Ts>
  struct overloaded : Ts...
  {
    using Ts::operator()...;
  };
}