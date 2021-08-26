#pragma once

namespace hhctrl::core::scheduler
{
struct RepeatedTaskRestore {};
struct OneShotTaskRestore{};

struct Restore
{
  virtual ~Restore() = default;
  virtual void restore(const RepeatedTaskRestore&) = 0;
  virtual void restore(const OneShotTaskRestore&) = 0;
};

template<class T>
struct GenericRestore : Restore
{
  template<class... TArgs>
  GenericRestore(TArgs&&... args)
    : handler{std::forward<TArgs...>(args...)}
  {}

  void restore(const RepeatedTaskRestore& arg) override
  {
    handler(arg);
  }
  void restore(const OneShotTaskRestore& arg) override
  {
    handler(arg);
  }

  T handler;
};
}