#pragma once

#include <string>
#include <optional>
#include <boost/uuid/uuid.hpp>

namespace hhctrl::core::scheduler
{
  struct TaskEntity
  {
    using Id_t = boost::uuids::uuid;

    Id_t id;
    std::string module_name;
    uint64_t timestamp;
  };

  class TaskStore
  {
  public:
    virtual ~TaskStore() = default;
    virtual void add(TaskEntity) = 0;
    virtual std::optional<TaskEntity> find(const TaskEntity::Id_t&) const = 0;
    virtual void remove(const TaskEntity::Id_t&) = 0;
  };
}