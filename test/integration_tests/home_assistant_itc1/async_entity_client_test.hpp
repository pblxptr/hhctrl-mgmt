#pragma once

#include <string>

namespace test {
  struct TestParams
  {
    std::string server_address;
    int server_port;
    size_t number_of_clients;
    size_t number_of_messages_per_client;
  };

  std::string rand_client_id();
  size_t number_of_messages_per_client();
  void fail(const std::string& client_id);
  void mark_ready(const std::string& client_id);
  bool marked_ready(const std::string& client_id);
  void notify_received_by(const std::string& client_id);
  void notify_sent_for(const std::string& client_id);

}
