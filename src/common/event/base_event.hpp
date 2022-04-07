#pragma once

#include <common/event/event_id.hpp>
#include <common/event/event_id_generator.hpp>

namespace common::event
{
  class BaseEvent
  {
  public:
    explicit BaseEvent(EventId_t event_id)
      : event_id_{event_id}
    {}

    EventId_t id() const
    {
      return event_id_;
    }

  private:
    EventId_t event_id_;
  };

  template<class DerivedEvent>
  class GenericEvent : public BaseEvent
  {
  protected:
    friend DerivedEvent;

    GenericEvent() : BaseEvent(EventIdGenerator::get<DerivedEvent>())
    {}
  };
}