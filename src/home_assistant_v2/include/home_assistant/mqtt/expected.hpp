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
#include <fmt/format.h>
#include <async_mqtt/exception.hpp>

namespace mgmt::home_assistant::mqtt {
  using Error = async_mqtt::system_error;

  template <typename T>
  using Expected = tl::expected<T, Error>;

  using Unexpected = tl::unexpected<Error>;
}
