////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Patryk Biel
/// @date 25.08.23
/// @copyright TRUMPF Huettinger
///
/// Copyright (c) 2019-2023, TRUMPF Huettinger
/// All rights reserved.
///
////////////////////////////////////////////////////////////////////////

#pragma once

#include <tl/expected.hpp>
#include <system_error>

namespace mgmt::home_assistant::v2 {
  template <typename T>
  using Expected = tl::expected<T, std::error_code>;

  using Unexpected = tl::unexpected<std::error_code>;

}
