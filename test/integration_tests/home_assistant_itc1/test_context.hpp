//
// Created by bielpa on 09.09.22.
//

#pragma once

#include <string>
#include <cassert>
#include <spdlog/spdlog.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/awaitable.hpp>
#include "test_params.hpp"

struct ClientWrapper;

class TestContext
{
  struct Messages
  {
    size_t sent_for{};
    size_t received_by{};
  };

public:
  TestContext(boost::asio::io_context& ioc, TestParams params);
  TestContext(TestContext&&) = default;
  TestContext& operator=(TestContext&&) = delete;
  TestContext(const TestContext&) = delete;
  TestContext& operator=(const TestContext&) = delete;
  ~TestContext();// On purpose in order to avoid inlining destructors for ClientWrapper objects
  std::string rand_client_id() const;
  size_t number_of_messages_per_client() const;
  void fail(const std::string& client_id);
  void mark_ready(const std::string& client_id);
  bool marked_ready(const std::string& client_id);
  void notify_received_by(const std::string& client_id);
  void notify_sent_for(const std::string& client_id);
  void run();

private:
  void run_clients();
  bool can_finish() const;
  void finish() const;

private:
  boost::asio::io_context& ioc_;
  TestParams params_;
  std::unordered_map<std::string, Messages> messages_;
  std::vector<std::unique_ptr<ClientWrapper>> clients_;
};
