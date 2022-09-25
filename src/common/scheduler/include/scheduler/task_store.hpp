#pragma once

#include <string>
#include <optional>
#include <boost/uuid/uuid.hpp>

namespace common::scheduler {
struct TaskEntity
{
  using Id_t = boost::uuids::uuid;

  Id_t id;
  std::string owner;
  std::int64_t timestamp;
};

class ITaskStore
{
public:
  ITaskStore() = default;
  ITaskStore(ITaskStore&&) noexcept = default;
  ITaskStore& operator=(ITaskStore&&) noexcept = default;
  ITaskStore(const ITaskStore&) = default;
  ITaskStore& operator=(const ITaskStore&) = default;
  virtual ~ITaskStore() = default;

  virtual void add(const TaskEntity&) = 0;
  virtual bool exist(const TaskEntity::Id_t&) const = 0;
  virtual std::optional<TaskEntity> find(const TaskEntity::Id_t&) const = 0;
  virtual void remove(const TaskEntity::Id_t&) = 0;
};
}// namespace common::scheduler
