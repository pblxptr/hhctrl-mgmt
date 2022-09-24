#pragma once

#include <boost/signals2/signal.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/system_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <functional>
#include <variant>

#include <coro/co_spawn.hpp>
#include <event/base_event.hpp>
#include <event/event.hpp>
#include <event/logger.hpp>
#include <utils/overloaded.hpp>
#include <utils/capture_fwd.hpp>
namespace common::event {
class AsyncEventBus
{
  using IoContex_t = boost::asio::io_context;
  using ThreadPool_t = boost::asio::thread_pool;
  using System_t = boost::asio::system_executor;
  using Executor_t = std::variant<
    std::reference_wrapper<IoContex_t>,
    std::reference_wrapper<ThreadPool_t>,
    std::reference_wrapper<System_t>>;

  using Slot_t = boost::signals2::signal<void(const BaseEvent&)>;

public:
  template<class Executor>
  explicit AsyncEventBus(Executor& executor)
    : executor_{ std::ref(executor) }
  {}

  // movable
  AsyncEventBus(AsyncEventBus&&) = default;
  AsyncEventBus& operator=(AsyncEventBus&&) = default;
  // non-copyable
  AsyncEventBus(const AsyncEventBus&) = delete;
  AsyncEventBus& operator=(const AsyncEventBus&) = delete;

  ~AsyncEventBus() = default;

  /*TODO: Once the event is going to be dispatched, there will created separate coroutine for every subscriber.
  Maybe it would be better to call all subscribers for the particular event in single coroutine?
  It could avoid copying event object for each subscriber just before the handler is invoked.
  Current:
    - dispatch event 1
    - dispatch_in_coro(event_1_sub_1 (copy event))
    - dispatch_in_coro(event_1_sub_2 (copy event))
    - dispatch_in_coro(event_1_sub_3 (copy event))
  Proposed:
    - dispatch event 1 (copy event)
      - dispatch_in_coro(event1_sub1, event1_sub2, event1_sub3);
  */
  template<Event E, class Handler>
  requires AsyncEventHandler<Handler, E> && Event<E>
  auto subscribe(Handler&& handler)// TODO(pp): Add check that verifies that object passed by value is copyable
  {
    using Event_t = std::decay_t<E>;

    const auto event_id = EventIdGenerator::get<Event_t>();

    common::logger::get(common::event::Logger)->debug("AsyncEventBus::{}, event id: {}", __FUNCTION__, event_id);

    // TODO(pp): These are just a few wrappers, that wrap slot callable needed by boost::signals, coroutine spawning, and finally event handler invocation.
    // capture_fwd is needed in order to preserve value category of handler that has passed. Otherwise it would be taken by value.
    auto slot_wrapper = [this, handler_as_tuple = common::utils::capture_fwd(std::forward<Handler>(handler)), event_id]
      // auto slot_wrapper = [this, handler_as_tuple = std::forward<Handler>(handler), event_id]
      (const BaseEvent& base_event) mutable {
        if (event_id != base_event.event_id()) {
          throw std::runtime_error("Event cannot be handled");
        }
        const auto& event = static_cast<const Event_t&>(base_event);
        auto spawn_coro_wrapper = [handler_as_tuple = std::move(handler_as_tuple), &event](auto&& executor) mutable {
          // TODO(pp): Why mutable below? Explanation:
          // Without mutable, handler is taken by const Handler& thus it's required that the function call operator() needs to have const in signature
          // what is not always applicable. Possible enhancement???
          auto event_handler_wrapper = [handler_as_tuple = std::move(handler_as_tuple)](Event_t event) mutable -> boost::asio::awaitable<void> {// Take event by copy
            common::logger::get(common::event::Logger)->debug("AsyncEventBus, before dispatch handler id: {}", event.event_id());
            co_await std::invoke(std::get<0>(handler_as_tuple), event);
            common::logger::get(common::event::Logger)->debug("AsyncEventBus, after dispatch handler id: {}", event.event_id());
          };

          boost::asio::co_spawn(executor.get(), event_handler_wrapper(event), common::coro::rethrow);
        };
        std::visit(spawn_coro_wrapper, executor_);
      };

    auto& slot = handlers_[event_id];
    return slot.connect(std::move(slot_wrapper));
  }

  template<Event E>
  void publish(const E& event)
  {
    common::logger::get(common::event::Logger)->debug("AsyncEventBus::{}, event id: {}", __FUNCTION__, event.event_id());

    auto& handler = handlers_[event.event_id()];
    handler(event);
  }

private:
  Executor_t executor_;
  std::unordered_map<EventId_t, Slot_t> handlers_;
};

}// namespace common::event
