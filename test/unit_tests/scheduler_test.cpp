#include "scheduler/scheduler.hpp"

#include <catch2/catch_test_macros.hpp>

#include "mock/gmock_task_store.hpp"

using namespace common::scheduler;

TEST_CASE("Tasks can be addded to scheduler")
{
  auto io_context = boost::asio::io_context{};
  auto task_store = GMockTaskStore{};
  auto sut = Scheduler{ io_context, task_store };

  SECTION("adding anoynomous tasks should make it active")
  {
    const auto task_id = sut.every(std::chrono::seconds(10), []() {});

    REQUIRE(sut.is_task_active(task_id));
  }

  SECTION("adding named taks should make it active")
  {
    const auto task_id = sut.every("named_task", std::chrono::seconds(10), []() {});

    REQUIRE(sut.is_task_active(task_id));
  }

  SECTION("addig anonymous and named tasks for the same duration should succeed")
  {
    const auto anonymous_task_id = sut.every(std::chrono::seconds(10), []() {});
    const auto named_task_id = sut.every("named_task", std::chrono::seconds(10), []() {});

    REQUIRE(sut.is_task_active(anonymous_task_id));
    REQUIRE(sut.is_task_active(named_task_id));
  }
}

TEST_CASE("Tasks can't be added to scheduler in certain circumstances")
{
  auto io_context = boost::asio::io_context{};
  auto task_store = GMockTaskStore{};
  auto sut = Scheduler{ io_context, task_store };

  SECTION("adding anoynomous tasks with the same duration twice should throw")
  {
    auto add_task = [&sut]() { sut.every(std::chrono::seconds(10), []() {}); };

    REQUIRE_NOTHROW(add_task());
    REQUIRE_THROWS(add_task());
  }

  SECTION("adding named tasks with the same duration twice should throw")
  {
    auto add_task = [&sut]() { sut.every("named-task", std::chrono::seconds(10), []() {}); };

    REQUIRE_NOTHROW(add_task());
    REQUIRE_THROWS(add_task());
  }

  SECTION("adding everydays at task without timezone should throw")
  {

    // Missing timezone in time should be e.g. "20:30:32 Europe/Warsaw"
    auto add_task = [&sut]() { sut.every(DaysAt{ std::chrono::days(1), "20:30:32" }, []() {}); };

    REQUIRE_THROWS(add_task());
  }

  SECTION("adding everydays at task with non-existing timezone should throw")
  {
    auto add_task = [&sut]() { sut.every(DaysAt{ std::chrono::days(1), "20:30:32 Fake/Timezone" }, []() {}); };

    REQUIRE_THROWS(add_task());
  }
}
