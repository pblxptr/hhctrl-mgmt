#pragma once

#include <concepts>
#include <type_traits>
#include <boost/asio/awaitable.hpp>

#include <event/base_event.hpp>

namespace common::event {
template<class Handler, class Arg>
concept AsyncEventHandler = requires(Handler&& handler, const Arg& arg)
{
  {
    std::invoke(std::forward<Handler>(handler), arg)
    } -> std::same_as<boost::asio::awaitable<void>>;
};

template<class E>
concept Event = requires(E e)
{
  requires std::is_base_of_v<GenericEvent<E>, E>;
};

template<Event E>
std::string to_string(const E& e)
{
  using std::to_string;

  return to_string(e.event_id());
}
}// namespace common::event
