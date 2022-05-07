#include <hw/platform_device/pdtree.hpp>

#include <fstream>

namespace hw::platform_device
{
  std::string pdtree_to_string(const PdTreeValue_t& val)
  {
    return val.as_string().c_str();
  }

  boost::json::value load_pdtree_file(const std::string& file_path)
  {
    auto file = std::ifstream{file_path};

    if (file.bad()) {
      throw std::runtime_error(fmt::format("Cannot open platform device file: {}", file_path));
    }

    auto parser = boost::json::stream_parser{};
    auto ec = boost::json::error_code{};

    do {
      auto buffer = std::array<char, 4096>{};
      file.read(buffer.begin(), buffer.size());
      parser.write(buffer.data(), file.gcount(), ec);
    } while(!file.eof());

    if (ec) {
      return nullptr;
    }
    parser.finish(ec);

    if (ec) {
      return nullptr;
    }
    return parser.release();
  }
}