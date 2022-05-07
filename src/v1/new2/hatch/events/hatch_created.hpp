#pragma once

#include <common/event/base_event.hpp>

namespace mgmt::event
{
  class HatchCreated : public common::event::GenericEvent<HatchCreated>
  {

  };
}