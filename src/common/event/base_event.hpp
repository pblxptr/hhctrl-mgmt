#pragma once

#include <common/event/event_id.hpp>
#include <common/event/event_id_generator.hpp>

namespace common::event {
class BaseEvent
{
protected:
  explicit BaseEvent(EventId_t event_id)
    : event_id_{ event_id }
  {}

public:
  EventId_t event_id() const
  {
    return event_id_;
  }

private:
  EventId_t event_id_{};
};

template<class DerivedEvent>
class GenericEvent : public BaseEvent
{
  // Consider leaving this protected or remove it.
  // In case protected and friends remains, it prohibits from wrong DerivedEvent being passed in the derived classs defintion
  // But it has its drawbacks - usually derived class could be an aggregate type with all properties being public, but then
  //"friend DerivedEvent" would not work. Instad calling point (like main func)  should be used in order to make it working.
  // Other solution is to prohibit derived type from being an aggregate. Usually ctor = default() defintion in derived class is sufficient
  // But it also has its drawbacks. One of them is that, when the class is not an aggregate type, we can't use aggregate initialization
  // and need to use ctor or member by member initialiazation instead.
protected:
  friend DerivedEvent;

  GenericEvent() : BaseEvent{ EventIdGenerator::get<DerivedEvent>() }
  {}
};
}// namespace common::event