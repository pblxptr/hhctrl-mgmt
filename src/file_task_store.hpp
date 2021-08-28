#pragma once

#include "task_store.hpp"

#include <vector>

namespace hhctrl::core::scheduler
{
  class FileTaskStore : public TaskStore
  {
  public:
    explicit FileTaskStore(std::string);
    void add(TaskEntity) override;
    std::optional<TaskEntity> find(const TaskEntity::Id_t&) const override;
    void remove(const TaskEntity::Id_t&) override;
  private:
    void load();
    void store();
  private:
    std::string filepath_;
    std::vector<TaskEntity> cached_tasks_;
  };
}