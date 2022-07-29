#pragma once

#include <common/command/requirements.hpp>
#include <common/command/command_handler_wrapper.hpp>
#include <common/command/command_id_generator.hpp>
#include <common/traits/type_tag.hpp>


namespace common::command {
class AsyncCommandDispatcher
{
public:
  template<class Command, class Handler>
  requires AsyncEventHandler<Handler, Command> && CommandCompatible<Command>
  void add_handler(Handler&& handler)
  {
    using Command_t = std::decay_t<Command>;

    const auto command_id_generator = CommandIdGenerator::get<Command_t>();

    if (handlers_.contains(command_id_generator)) {
      throw std::runtime_error("Handler for requested command already exists");
    }

    handlers_.insert({ command_id_generator, AsyncHandlerWrapper{ common::traits::TypeTag<Command_t>{}, std::forward<Handler>(handler) } });
  }

  template<CommandCompatible Command>
  boost::asio::awaitable<void> async_dispatch(Command&& command)
  {
    using Command_t = std::decay_t<Command>;

    auto handler_it = handlers_.find(CommandIdGenerator::get<Command_t>());

    if (handler_it == handlers_.end()) {
      throw std::runtime_error("Cannot find handler for requested command");
    }

    auto& [command_id, handler] = *handler_it;

    co_await handler.async_dispatch(std::forward<Command>(command));
  }

private:
  std::unordered_map<CommandId_t, AsyncHandlerWrapper> handlers_;
};
}// namespace common::command