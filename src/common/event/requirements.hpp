#pragma once

#include <concepts>
#include <type_traits>
#include <boost/asio/awaitable.hpp>

#include <common/event/base_event.hpp>

namespace common::event
{
  template<class Handler, class Arg>
  concept AsyncHandlerCompatible = requires(Handler&& handler, const Arg& arg)
  {
    { std::invoke(std::forward<Handler>(handler), arg) } -> std::same_as<boost::asio::awaitable<void>>;
  };

  template<class Event>
  concept EventCompatible = requires(Event e)
  {
    requires std::is_base_of_v<GenericEvent<Event>, Event>;
  };
}