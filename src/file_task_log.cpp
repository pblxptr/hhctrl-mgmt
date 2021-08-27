#include "file_task_log.hpp"

#include <vector>
#include <fstream>
#include <spdlog/spdlog.h>


namespace
{
std::string serialize(const hhctrl::core::scheduler::TaskLogEntry& e)
{
  auto entry_line = fmt::format("{} {}",
    e.name(),
    std::chrono::time_point_cast<std::chrono::milliseconds>(e.timepoint())
      .time_since_epoch()
      .count()
  );

  return entry_line;
}

hhctrl::core::scheduler::TaskLogEntry deserialize(const std::string& str)
{
  auto iss = std::istringstream{str};
  std::string name{};
  std::int64_t timestamp;
  iss >> name >> timestamp;

  return hhctrl::core::scheduler::TaskLogEntry{
    std::move(name),
    utils::datetime::from_timestamp(std::chrono::milliseconds(timestamp))
  };
}
}

namespace hhctrl::core::scheduler
{

static std::vector<TaskLogEntry> entries;


FileTaskLog::FileTaskLog(std::string fname)
  : fname_{std::move(fname)}
{
  load();
}

void FileTaskLog::add(TaskLogEntry entry)
{
  spdlog::debug("FileTasklog::add()");

  entries.push_back(std::move(entry));
  store();
  load();
}

void FileTaskLog::erase(const std::string& name)
{
  spdlog::debug(fmt::format("FileTasklog::erase() by name: {}", name));

  auto entry = std::find_if(entries.begin(), entries.end(), [&name](const auto& x) { return x.name() == name; });

  if (entry == entries.end()) {
    throw std::runtime_error("Entry not found");
  }

  entries.erase(entry);

  store();
  load();
}

std::optional<TaskLogEntry> FileTaskLog::find(const std::string& name) const
{
  auto entry = std::find_if(entries.begin(), entries.end(), [&name](const auto& x) { return x.name() == name; });

  if (entry == entries.end()) {
    return std::nullopt;
  }

  return *entry;
}
bool FileTaskLog::exist(const std::string& name) const
{
  fmt::print("(FileTaskLog) exist\n");
  return std::find_if(entries.begin(), entries.end(), [&name](const auto& x) { return x.name() == name; }) != entries.end();
}

void FileTaskLog::load()
{
  entries.clear();

  auto file = std::ifstream(fname_, std::ios_base::in);

  if (!file) {
    throw std::runtime_error("File does not exist");
  }

  std::string line;
  spdlog::debug("FileTaskLog::load() deserialized entries:");

  while (std::getline(file, line)) {
    auto entry = deserialize(line);
    spdlog::debug(fmt::format(" - name:{} tp:{}", entry.name(), utils::datetime::to_string(entry.timepoint())));

    entries.push_back(std::move(entry));
  }
}

void FileTaskLog::store()
{
  auto file = std::ofstream{fname_, std::ios_base::out | std::ios_base::trunc};

  if (!file) {
    throw std::runtime_error{"File does not exist"};
  }
  spdlog::debug("FileTaskLog::store() serialized entries:");
  for (const auto& e : entries) {
    const auto line = serialize(e);
    spdlog::debug(fmt::format(" - {}", line));

    file << line << '\n';
  }
}
}