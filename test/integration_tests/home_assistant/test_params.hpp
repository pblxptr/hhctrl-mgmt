#pragma once

#include <string>

struct TestParams
{
  std::string server_address;
  int server_port;
  size_t number_of_clients;
  size_t number_of_messages_per_client;
};
