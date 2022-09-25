#pragma once

#include <concepts>
#include <boost/asio/awaitable.hpp>

#include <command/base_command.hpp>

namespace common::command {
template<class Handler, class Arg>
concept AsyncEventHandler = requires(Handler&& handler, const Arg& arg)
{
  {
    std::invoke(std::forward<Handler>(handler), arg)
    } -> std::same_as<boost::asio::awaitable<void>>;
};

template<class Command>
concept CommandCompatible = requires(Command c)
{
  requires std::is_base_of_v<GenericCommand<std::decay_t<Command>>, std::decay_t<Command>>;
};
}// namespace common::command