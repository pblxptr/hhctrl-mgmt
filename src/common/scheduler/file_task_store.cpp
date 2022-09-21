#include <scheduler/file_task_store.hpp>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <boost/json.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;
namespace fs = std::filesystem;

namespace common::scheduler {
// Serialize
void tag_invoke(json::value_from_tag /* unused */, json::value& jvalue, const TaskEntity& task)
{
  using std::to_string;
  jvalue = {
    { "id", to_string(task.id) },
    { "owner", task.owner },
    { "timestamp", task.timestamp }
  };
}

// Deserialize
TaskEntity tag_invoke(json::value_to_tag<TaskEntity> /* unused */, const json::value& jvalue)
{
  auto string_gen = boost::uuids::string_generator{};
  const json::object& obj = jvalue.as_object();
  return TaskEntity{
    string_gen(json::value_to<std::string>(obj.at("id"))),
    json::value_to<std::string>(obj.at("owner")),
    json::value_to<std::int64_t>(obj.at("timestamp"))
  };
}


FileTaskStore::FileTaskStore(std::string fpath)
  : filepath_{ std::move(fpath) }
{
  if (!fs::exists(filepath_)) {
    auto file = std::fstream{ filepath_, std::ios::out };
    store();
  }

  load();
}

void FileTaskStore::add(const TaskEntity& entity)
{
  auto task = std::find_if(cached_tasks_.begin(), cached_tasks_.end(), [&entity](const auto& xtask) {
    return entity.id == xtask.id;
  });

  if (task != cached_tasks_.end()) {
    throw std::runtime_error("Task already exist.");
  }

  cached_tasks_.push_back(entity);
  store();
  load();
}

bool FileTaskStore::exist(const TaskEntity::Id_t& task_id) const
{
  return std::find_if(cached_tasks_.begin(), cached_tasks_.end(), [&task_id](const auto& xtask) {
    return task_id == xtask.id;
  }) != cached_tasks_.end();
}

std::optional<TaskEntity> FileTaskStore::find(const TaskEntity::Id_t& task_id) const// TODO(pp): Consider returning different type
{
  auto task = std::find_if(cached_tasks_.begin(), cached_tasks_.end(), [&task_id](const auto& xtask) {
    return task_id == xtask.id;
  });

  if (task == cached_tasks_.end()) {
    return std::nullopt;
  }

  return { *task };
}

void FileTaskStore::remove(const TaskEntity::Id_t& task_id)
{
  auto task = std::find_if(cached_tasks_.begin(), cached_tasks_.end(), [&task_id](const auto& xtask) {
    return task_id == xtask.id;
  });

  if (task == cached_tasks_.end()) {
    throw std::runtime_error("Task does not exist");
  }

  cached_tasks_.erase(task);
  store();
  load();
}

void FileTaskStore::load()
{
  auto file = std::fstream(filepath_, std::ios::in);
  if (!file) {
    throw std::runtime_error("Error while reading file.");
  }
  auto parser = json::stream_parser{};
  auto error_code = json::error_code{};

  do {
    auto buffer = std::array<char, 4096>{};
    file.read(buffer.begin(), buffer.size());
    parser.write(buffer.data(), file.gcount(), error_code);
  } while (!file.eof());

  if (error_code) {
    throw std::runtime_error("Error while reading file.");
  }
  parser.finish(error_code);

  if (error_code) {
    throw std::runtime_error("Error while parsing file. File may be malformed.");
  }

  // TODO(pp): Consider more efficient way to update values, monotonic buffers etc
  cached_tasks_ = json::value_to<std::vector<TaskEntity>>(parser.release());
}

void FileTaskStore::store()
{
  auto file = std::fstream(filepath_, std::ios::out);
  if (!file) {
    throw std::runtime_error("Error while writing to file.");
  }

  file << json::serialize(json::value_from(cached_tasks_));
}

}// namespace common::scheduler
