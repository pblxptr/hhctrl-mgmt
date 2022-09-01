#include <catch2/catch_all.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>


#include <command/dispatcher.hpp>
#include <command/base_command.hpp>

struct TestCommand1 : common::command::GenericCommand<TestCommand1>
{
  explicit TestCommand1(int num)
    : number{ num }
  {}

  const int number{};
};

struct TestCommand2 : common::command::GenericCommand<TestCommand2>
{
};

SCENARIO("Handlers can be added to dispatcher")
{
  GIVEN("An AsyncCommandDispatcher")
  {
    auto dispatcher = common::command::AsyncCommandDispatcher{};

    WHEN("adding a handler")
    {
      auto add_handler = [&dispatcher]() {
        dispatcher.add_handler<TestCommand1>([](const TestCommand1&) -> boost::asio::awaitable<void> { co_return; });
      };

      THEN("no exception is thrown")
      {
        REQUIRE_NOTHROW(add_handler());
      }
    }

    WHEN("adding multiple handlers for different types of commands")
    {
      auto add_handler1 = [&dispatcher]() {
        dispatcher.add_handler<TestCommand1>([](const TestCommand1&) -> boost::asio::awaitable<void> { co_return; });
      };
      auto add_handler2 = [&dispatcher]() {
        dispatcher.add_handler<TestCommand2>([](const TestCommand2&) -> boost::asio::awaitable<void> { co_return; });
      };

      THEN("no exception is throw")
      {
        REQUIRE_NOTHROW(add_handler1());
        REQUIRE_NOTHROW(add_handler2());
      }
    }

    WHEN("adding handler for the same type of command twice")
    {
      auto add_handler = [&dispatcher]() {
        dispatcher.add_handler<TestCommand1>([](const TestCommand1&) -> boost::asio::awaitable<void> { co_return; });
      };

      THEN("exception should be thrown")
      {
        REQUIRE_NOTHROW(add_handler());
        REQUIRE_THROWS(add_handler());
      }
    }
  }
}

SCENARIO("Handlers should be executed")
{
  GIVEN("An AsyncCommandDispatcher with configured handlers")
  {
    auto dispatcher = common::command::AsyncCommandDispatcher{};
    auto ioc = boost::asio::io_context{};
    auto command_execution_result = 0;
    dispatcher.add_handler<TestCommand1>([&command_execution_result](const TestCommand1& command) -> boost::asio::awaitable<void> {
      command_execution_result = command.number;
      co_return;
    });

    WHEN("dispatching a command that has appropriate handlers registered")
    {
      constexpr auto command_value = 100;// it can be any value

      /*  TODO: This seems to be kinda interesting, needs investigation, the same may happen for EventHandler, or it may be a bug of asan dealing with coroutines
       *  - X: When the async_dispatch takes parameter by UNIVERSAL REF and command is created in place of function call then we've got stack-use-after-scope-error
       *  - V: When the async_dispatch takes parameter by UNIVERSAL REF and command is passed by lvalue ref then everything's fine
       *  - V: When the async_dispatch takes parameter by VALUE and command is created in place of function call then everything's fine
       *  - V: When the async_dispatch takes parameter by VALUE and command is passed by lvalue ref then then everything's fine
       */

      boost::asio::co_spawn(ioc, dispatcher.async_dispatch(TestCommand1{command_value}), boost::asio::detached);
      ioc.run_one();

      THEN("handler gets invoked")
      {
        REQUIRE(command_execution_result == command_value);
      }
    }
  }
}