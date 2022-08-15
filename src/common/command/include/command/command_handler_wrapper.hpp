#pragma once

#include <command/requirements.hpp>
#include <traits/type_tag.hpp>
#include <command/base_command.hpp>

namespace common::command {
class AsyncHandlerWrapper
{
public:
  template<class Command, class Handler>
  requires AsyncEventHandler<Handler, Command> && CommandCompatible<Command>
    AsyncHandlerWrapper(common::traits::TypeTag<Command>, Handler&& handler)
  {
    static_assert(std::is_base_of_v<GenericCommand<Command>, Command>, "Command must derive from GenericCommand<>");

    handler_wrapper_ = [h = std::forward<Handler>(handler)](const BaseCommand& base_cmd) -> boost::asio::awaitable<void> {
      if (not base_cmd.id() == CommandIdGenerator::get<Command>()) {
        throw std::runtime_error("Command cannot be handled by this particular handler.");
      }

      co_await h(static_cast<const Command&>(base_cmd));
    };
  }

  template<CommandCompatible Command>
  boost::asio::awaitable<void> async_dispatch(Command&& command) const
  {
    co_await handler_wrapper_(std::forward<Command>(command));
  }

private:
  std::function<boost::asio::awaitable<void>(const BaseCommand&)> handler_wrapper_;
};
}// namespace common::command