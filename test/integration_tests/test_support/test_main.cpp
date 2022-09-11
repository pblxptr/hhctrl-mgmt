
#include <gmock/gmock.h>
#include <catch2/catch_session.hpp>

#include <test_support/test_setup.hpp>
#include <test_support/test_config.hpp>

__attribute__((weak)) void test_spec_setup()
{
  throw std::runtime_error{"dupa"};
}

int main(int argc, char* argv[])
{
  using namespace Catch::Clara;

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  auto session = Catch::Session{};

  auto& config = TestConfig::get();
  test_spec_setup();
  config.apply(session);

  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
    return returnCode;

  return session.run(argc, argv);
}
