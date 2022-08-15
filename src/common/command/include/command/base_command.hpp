#pragma once

#include <cstddef>

#include <command/command_id.hpp>
#include <command/command_id_generator.hpp>

namespace common::command {
class BaseCommand
{
protected:
  explicit BaseCommand(CommandId_t command_id)
    : id_{ command_id }
  {}

public:
  BaseCommand(const BaseCommand&) = delete;
  BaseCommand(BaseCommand&&) = default;

  BaseCommand& operator=(const BaseCommand&) = delete;
  BaseCommand& operator=(BaseCommand&&) = default;

  CommandId_t id() const
  {
    return id_;
  }

private:
  CommandId_t id_;
};

template<class DerivedCommand>
class GenericCommand : public BaseCommand
{
  // Protected ctor and friend 'DerivedCommand' serve as a mechanism that prevent from
  // wrong type being passsed as a DerivedCommand.
  // E.g. one could create: class Derived : public GenericCommand<DerivedSTH> {}
  // which would casue undefined behaviour later in the code.
  // Unfortunately due to this constraint, each derived class cannot be an aggregate class thus needs to have e.g. a
  // user-provided constructor.
protected:
  friend DerivedCommand;
  GenericCommand() : BaseCommand{ CommandIdGenerator::get<DerivedCommand>() }
  {}
};
}// namespace common::command