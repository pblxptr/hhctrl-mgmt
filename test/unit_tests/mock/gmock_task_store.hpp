#include "../../../src/common/scheduler/include/scheduler/task_store.hpp"

#include "../../../../../../.conan/data/gtest/cci.20210126/_/_/package/63868df56b76903d4ad40ecbd5b2e8238cee50c9/include/gmock/gmock.h"

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
