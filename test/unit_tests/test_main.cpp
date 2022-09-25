
#include <gmock/gmock.h>
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[])
{
  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  return Catch::Session().run(argc, argv);
}
