#pragma once

#include <atomic>

#include <event/event_id.hpp>

namespace common::event {
class EventIdGenerator
{
public:
  template<class E>
  static EventId_t get()
  {
    using Event_t = std::decay_t<E>;

    return id<Event_t>();
  }

private:
  template<class E>
  static EventId_t id()
  {
    static auto event_id = Counter++;

    return event_id;
  }

  static inline std::atomic<EventId_t> Counter{ 1 };// NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
};
}// namespace common::event
