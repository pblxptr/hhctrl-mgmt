#include <main_board/platform/pdtree.hpp>

#include <fstream>

namespace mgmt::platform_device {
std::string pdtree_to_string(const PdTreeValue_t& val)
{
  return val.as_string().c_str();
}

boost::json::value pdtree_load(const std::string& file_path)
{
  auto file = std::ifstream{ file_path };

  if (file.bad()) {
    throw std::runtime_error(fmt::format("Cannot open platform device file: {}", file_path));
  }

  auto parser = boost::json::stream_parser{};
  auto ec = boost::json::error_code{};

  do {
    auto buffer = std::array<char, 4096>{};
    file.read(buffer.begin(), buffer.size());
    parser.write(buffer.data(), file.gcount(), ec);
  } while (!file.eof());

  if (ec) {
    return nullptr;
  }
  parser.finish(ec);

  if (ec) {
    return nullptr;
  }
  return parser.release();
}
}// namespace mgmt::platform_device