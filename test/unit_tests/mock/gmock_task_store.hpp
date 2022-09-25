#include <scheduler/task_store.hpp>

#include <gmock/gmock.h>

class GMockTaskStore : public common::scheduler::ITaskStore
{
public:
  MOCK_METHOD(void, add, (const common::scheduler::TaskEntity&), (override));
  MOCK_METHOD(bool, exist, (const common::scheduler::TaskEntity::Id_t&), (const override));
  MOCK_METHOD(std::optional<common::scheduler::TaskEntity>, find, (const common::scheduler::TaskEntity::Id_t&), (const override));
  MOCK_METHOD(void, remove, (const common::scheduler::TaskEntity::Id_t&), (override));
};
