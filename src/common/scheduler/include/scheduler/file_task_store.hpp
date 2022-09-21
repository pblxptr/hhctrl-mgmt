#pragma once

#include "task_store.hpp"

#include <vector>

namespace common::scheduler {
class FileTaskStore : public TaskStore
{
public:
  explicit FileTaskStore(std::string);
  void add(const TaskEntity&) override;
  bool exist(const TaskEntity::Id_t&) const override;
  std::optional<TaskEntity> find(const TaskEntity::Id_t&) const override;
  void remove(const TaskEntity::Id_t&) override;

private:
  void load();
  void store();

private:
  std::string filepath_;
  std::vector<TaskEntity> cached_tasks_;
};
}// namespace common::scheduler
