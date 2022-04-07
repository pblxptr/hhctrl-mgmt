#pragma once

#include <boost/signals2/signal.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <functional>

#include <common/event/base_event.hpp>
#include <common/event/requirements.hpp>
#include <common/utils/overloaded.hpp>

namespace common::event
{
  class AsyncEventBus
  {
    using IoContex_t = boost::asio::io_context;
    using ThreadPool_t = boost::asio::thread_pool;
    using System_t = boost::asio::system_executor;
    using Executor_t = std::variant<
      std::reference_wrapper<IoContex_t>,
      std::reference_wrapper<ThreadPool_t>,
      std::reference_wrapper<System_t>
    >;

    using Slot_t = boost::signals2::signal<void (const BaseEvent&)>;
    using Connection_t = boost::signals2::connection;
  public:
    template<class Executor>
    explicit AsyncEventBus(Executor& executor)
      : executor_{std::ref(executor)}
    {}

    AsyncEventBus(const AsyncEventBus&) = delete;
    AsyncEventBus(AsyncEventBus&&) = default;
    AsyncEventBus& operator=(const AsyncEventBus&) = delete;
    AsyncEventBus& operator=(AsyncEventBus&&) = default;

    template<EventCompatible Event, class Handler>
      requires AsyncHandlerCompatible<Handler, Event> && EventCompatible<Event>
    Connection_t subscribe(Handler&& handler)
    {
      using Event_t = std::decay_t<Event>;

      auto async_wrapper = [handler = std::forward<Handler>(handler)](const BaseEvent& event)
        -> boost::asio::awaitable<void> {
        if (event.id() != EventIdGenerator::get<Event_t>()) {
          throw std::runtime_error("Event cannot be handled by handler");
        }

        //TODO: Consider taking a value of BaseEvent after cast.

        co_await handler(static_cast<const Event&>(event));
      };

      auto slot_wrapper = [this, async_wrapper = std::move(async_wrapper)](const BaseEvent& event) {
        std::visit(common::utils::overloaded {
          [&event, async_wrapper = std::move(async_wrapper)](std::reference_wrapper<IoContex_t> executor) {
            boost::asio::co_spawn(executor.get(), async_wrapper(event), boost::asio::detached);
          },
          [&event, async_wrapper = std::move(async_wrapper)](std::reference_wrapper<ThreadPool_t> executor) {
            boost::asio::co_spawn(executor.get(), async_wrapper(event), boost::asio::detached);
          },
          [&event, async_wrapper = std::move(async_wrapper)](std::reference_wrapper<System_t> executor) {
            boost::asio::co_spawn(executor.get(), async_wrapper(event), boost::asio::detached);
          }
        },
        executor_);
      };

      auto& slot = handlers_[EventIdGenerator::get<Event_t>()];
      return slot.connect(std::move(slot_wrapper));
    }

    template<EventCompatible Event>
    void publish(Event&& event)
    {
      using Event_t = std::decay_t<Event>;

      auto& handler = handlers_[EventIdGenerator::get<Event_t>()];
      handler(std::forward<Event>(event));
    }

  private:
    Executor_t executor_;
    std::unordered_map<EventId_t, Slot_t> handlers_;
  };

}