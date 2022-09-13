//
// Created by bielpa on 15.08.22.
//

#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>

#pragma once

namespace common::coro {
template<class Response, class Func, typename ResponseHandler = boost::asio::use_awaitable_t<>>
auto async_call(Func&& func, ResponseHandler&& handler = {})
{
  auto initiate = [func = std::forward<decltype(func)>(func)]<typename Handler>(Handler&& self) mutable {
    func([self = std::make_shared<Handler>(std::forward<Handler>(self))](const Response& r) {
      (*self)(r);
    });
  };
  return boost::asio::async_initiate<
    ResponseHandler,
    void(const Response&)>(
    initiate, handler);
}

template<class R, class Func, class CompletitionHandler = boost::asio::use_awaitable_t<>>
auto awaitable_call(Func&& func, CompletitionHandler&& handler = {})
{
  using Signature_t = void(std::exception_ptr, R);

  auto initiate = [func = std::forward<Func>(func)]<class Handler>(Handler&& self) mutable {
    std::invoke(func, [self = std::make_shared<Handler>(std::forward<Handler>(self))](auto&& r, auto&&... args) {
      (*self)(std::current_exception(), std::forward<decltype(r)>(r), std::forward<decltype(args)>(args)...);
    });
  };
  return boost::asio::async_initiate<CompletitionHandler, Signature_t>(initiate, handler);
}

template<class Func, class O, class... Args>
auto make_awaitable_for(Func&& func, O&& o, Args&&... args)
{
  return std::bind(std::forward<Func>(func),
    std::forward<O>(o),
    std::forward<Args>(args)...,
    std::placeholders::_1);
}
}// namespace common::coro
