#pragma once

#include "task_store.hpp"

#include <vector>

namespace common::scheduler {
class FileTaskStore : public ITaskStore
{
public:
  explicit FileTaskStore(std::string fpath);
  void add(const TaskEntity& entity) override;
  bool exist(const TaskEntity::Id_t& task_id) const override;
  std::optional<TaskEntity> find(const TaskEntity::Id_t& task_id) const override;
  void remove(const TaskEntity::Id_t& task_id) override;

private:
  void load();
  void store();

private:
  std::string filepath_;
  std::vector<TaskEntity> cached_tasks_;
};
}// namespace common::scheduler
