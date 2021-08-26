#pragma once

#include "task.hpp"
#include "datetime.hpp"

namespace hhctrl::core::scheduler
{
  class TaskLogEntry
  {
  public:
    TaskLogEntry(std::string name, Timepoint_t tp)
      : name_{std::move(name)}
      , tp_{std::move(tp)}
    {}

    const std::string& name() const { return name_; }
    const Timepoint_t timepoint() const { return tp_ ; }
  private:
    std::string name_;
    Timepoint_t tp_;
  };
}