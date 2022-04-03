#pragma once

#include <concepts>
#include <boost/asio/awaitable.hpp>

#include <common/command/base_command.hpp>

namespace common::command
{
  template<class Handler, class Arg>
  concept AsyncHandlerCompatible = requires(Handler&& handler, const Arg& arg)
  {
    { std::invoke(std::forward<Handler>(handler), arg) } -> std::same_as<boost::asio::awaitable<void>>;
  };

  template<class Command>
  concept CommandCompatible = requires(Command c)
  {
    requires std::is_base_of_v<GenericCommand<Command>, Command>;
  };
}