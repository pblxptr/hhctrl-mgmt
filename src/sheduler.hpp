#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <fmt/format.h>
#include <boost/asio.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "datetime.hpp"
#include "repeated_task.hpp"
#include "repeated_seconds.hpp"
#include "file_task_log.hpp"

namespace hhctrl::core::scheduler
{
  using seconds = std::chrono::seconds;
  using days = std::chrono::days;
  using at_time = std::string;

class Scheduler
{
public:
  explicit Scheduler(boost::asio::io_context& io);
  void add_task(std::string id, std::unique_ptr<Task> action);

  template<class... TArgs>
  void every(TArgs&&... args)
  {
    do_every(std::forward<TArgs>(args)...);
  }

  template<class TInverval, class THandler>
  void do_every(const TInverval& interval, THandler&& handler)
  {

  }

  template<class THandler>
  void do_every(std::string name, days interval, at_time expiry, THandler&& handler)
  {
    add_task(std::make_unique<scheduler::RepeatedTask>(
      io_,
      name,
      utils::datetime::parse_time(std::move(expiry)),
      std::move(interval),
      [this, name = name, handle = std::forward<THandler>(handler)]()
      {
        handle();
        task_completed(name);
      }
    ));
  }
  template<class TInverval, class THandler>
  void every(std::string name, const TInverval& interval, THandler&& handler)
  {
    add_task(std::make_unique<scheduler::RepeatedInterval<TInverval>>(
      io_,
      name,
      interval,
      [this, name = name, handle = std::forward<THandler>(handler)]()
      {
        handle();
        task_completed(name);
      }
    ));
  }

private:
  void add_task(std::unique_ptr<Task>);
  void task_completed(const std::string&);
  void schedule(std::unique_ptr<Task>);
  void add_to_log_if_not_exist(const Task&);
  bool is_task_active(const Task&);
  bool was_task_executed(const Task&);


private:
  boost::asio::io_context& io_;
  FileTaskLog task_log_;
  boost::uuids::random_generator_mt19937 id_gen_;
  std::vector<std::unique_ptr<Task>> active_tasks_;

};
}