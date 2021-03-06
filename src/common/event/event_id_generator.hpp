#pragma once

#include <common/event/event_id.hpp>

namespace common::event
{
  class EventIdGenerator
  {
  public:
    template<class Event>
    static EventId_t get()
    {
      using Event_t = std::decay_t<Event>;

      return id<Event_t>();
    }
  private:
    template<class Event>
    static EventId_t id()
    {
      static auto id = counter_++;

      return id;
    }

    static inline std::atomic<EventId_t> counter_;
  };
}