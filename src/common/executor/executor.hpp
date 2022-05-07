#pragma once

#include <variant>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/system_context.hpp>

namespace common::executor
{
  using IoContex_t = boost::asio::io_context;
  using ThreadPool_t = boost::asio::thread_pool;
  using System_t = boost::asio::system_executor;
  using Executor_t = std::variant<
    std::reference_wrapper<IoContex_t>,
    std::reference_wrapper<ThreadPool_t>,
    std::reference_wrapper<System_t>
  >;

  template<class Executor, class F>
  auto invoke_with_executor(Executor&& executor, F&& f)
  {
    return std::visit(overloaded {
      [f = std::forward<F>(f)](std::reference_wrapper<IoContex_t> executor) mutable {
        return std::invoke(std::forward<F>(f), executor.get());
      },
      [f = std::forward<F>(f)](std::reference_wrapper<ThreadPool_t> executor) mutable {
        return std::invoke(std::forward<F>(f), executor.get());
      },
      [f = std::forward<F>(f)](std::reference_wrapper<System_t> executor) mutable {
        return std::invoke(std::forward<F>(f), executor.get());
      }
    },
    std::forward<Executor>(executor));
  }
}