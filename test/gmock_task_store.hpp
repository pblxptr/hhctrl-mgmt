#include <common/scheduler/task_store.hpp>

#include <gmock/gmock.h>

namespace {
  using namespace common::scheduler;
}

class GMockTaskStore : public common::scheduler::TaskStore
{
public:
  MOCK_METHOD(void, add, (TaskEntity), (override));
  MOCK_METHOD(bool, exist, (const TaskEntity::Id_t&), (const override));
  MOCK_METHOD(std::optional<TaskEntity>, find, (const TaskEntity::Id_t&), (const override));
  MOCK_METHOD(void, remove, (const TaskEntity::Id_t&), (override));
};
