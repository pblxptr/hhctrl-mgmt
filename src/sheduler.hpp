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
#include "file_task_log.hpp"

namespace hhctrl::core::scheduler
{
template<class... Ts>
struct overload : Ts...
{
  using Ts::operator()...;
};

class Scheduler
{
public:
  explicit Scheduler(boost::asio::io_context& io);
  void add_task(std::string id, std::unique_ptr<Task> action);

  template<class THandler>
  void repeat_at(std::string name, const std::string& expiry, THandler&& handler)
  {
    auto id = id_gen_();

    add_task(std::make_unique<scheduler::RepeatedTask>(
      io_,
      id,
      std::move(name),
      utils::datetime::parse_time(expiry),
      [this, id = std::move(id), handle = std::forward<THandler>(handler)]()
      {
        handle();
        task_completed(id);
      }
    ));
  }
private:
  void add_task(std::unique_ptr<Task>);
  void task_completed(const boost::uuids::uuid&);
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