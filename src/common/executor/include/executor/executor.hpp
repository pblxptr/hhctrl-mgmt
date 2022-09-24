#pragma once

#include <variant>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/system_context.hpp>

namespace common::executor {
using IoContex_t = boost::asio::io_context;
using ThreadPool_t = boost::asio::thread_pool;
using System_t = boost::asio::system_executor;
using Executor_t = std::variant<
  std::reference_wrapper<IoContex_t>,
  std::reference_wrapper<ThreadPool_t>,
  std::reference_wrapper<System_t>>;
template<class Executor, class Func>
auto invoke(Executor&& executor, Func&& func)
{
  auto invoke_with_executor = [func = std::forward<Func>(func)](auto&& executor) -> decltype(auto) {
    return func(executor.get());
  };

  return std::visit(invoke_with_executor, std::forward<Executor>(executor));
}
}// namespace common::executor
