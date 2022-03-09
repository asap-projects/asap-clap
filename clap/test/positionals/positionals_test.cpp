//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/cli.h"
#include "clap/command.h"
#include "clap/option.h"

#include <common/compilers.h>

#include <array>
#include <memory>

#include "gmock/gmock-more-matchers.h"
#include "gtest/gtest.h"

// Disable compiler and linter warnings originating from the unit test framework
// and for which we cannot do anything. Additionally, every TEST or TEST_X macro
// usage must be preceded by a '// NOLINTNEXTLINE'.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__) && ASAP_HAS_WARNING("-Wused-but-marked-unused")
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wunused-member-function"
#endif
// NOLINTBEGIN(used-but-marked-unused)

using ::testing::Eq;
using ::testing::IsTrue;

namespace asap::clap {

namespace {

inline auto MakeBefore_1() -> PositionalOptionBuilder {
  return std::move(Option::Positional("BEFORE_1")
                       .About("first positional before rest")
                       .WithValue(ValueDescriptor<std::string>::Create()));
}

inline auto MakeBefore_2() -> PositionalOptionBuilder {
  return std::move(Option::Positional("BEFORE_2")
                       .About("second positional before rest")
                       .WithValue(ValueDescriptor<std::string>::Create()));
}

inline auto MakeRest() -> PositionalOptionBuilder {
  return std::move(Option::Rest()
                       .About("remaining positional arguments")
                       .WithValue(ValueDescriptor<std::string>::Create()));
}

inline auto MakeAfter_1() -> PositionalOptionBuilder {
  return std::move(Option::Positional("AFTER_1")
                       .About("first positional after rest")
                       .WithValue(ValueDescriptor<std::string>::Create()));
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, JustRest) {
  constexpr auto argc = 5;
  std::array<const char *, argc> argv{
      "/usr/bin/test-program.exe", "r_1", "r_2", "r_3", "r_4"};

  auto default_command = std::make_shared<Command>(Command::DEFAULT);
  default_command->WithPositionals(MakeRest());

  Cli cli;
  cli.ProgramName("positionals").WithCommand(default_command);
  const auto &matches = cli.Parse(argc, argv.data());

  const auto &v_rest = matches.ValuesOf(Option::key_rest);
  EXPECT_THAT(v_rest.size(), Eq(4));
  EXPECT_THAT(v_rest.at(0).GetAs<std::string>(), Eq("r_1"));
  EXPECT_THAT(v_rest.at(1).GetAs<std::string>(), Eq("r_2"));
  EXPECT_THAT(v_rest.at(2).GetAs<std::string>(), Eq("r_3"));
  EXPECT_THAT(v_rest.at(3).GetAs<std::string>(), Eq("r_4"));
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, BeforeRest) {
  constexpr auto argc = 5;
  std::array<const char *, argc> argv{
      "/usr/bin/test-program.exe", "b_1", "b_2", "r_1", "r_2"};

  auto default_command = std::make_shared<Command>(Command::DEFAULT);
  default_command->WithPositionals(MakeBefore_1(), MakeBefore_2(), MakeRest());

  Cli cli;
  cli.ProgramName("positionals").WithCommand(default_command);
  const auto &matches = cli.Parse(argc, argv.data());

  const auto &v_before_1 = matches.ValuesOf("BEFORE_1");
  EXPECT_THAT(v_before_1.size(), Eq(1));
  EXPECT_THAT(v_before_1.at(0).GetAs<std::string>(), Eq("b_1"));

  const auto &v_before_2 = matches.ValuesOf("BEFORE_2");
  EXPECT_THAT(v_before_2.size(), Eq(1));
  EXPECT_THAT(v_before_2.at(0).GetAs<std::string>(), Eq("b_2"));

  const auto &v_rest = matches.ValuesOf(Option::key_rest);
  EXPECT_THAT(v_rest.size(), Eq(2));
  EXPECT_THAT(v_rest.at(0).GetAs<std::string>(), Eq("r_1"));
  EXPECT_THAT(v_rest.at(1).GetAs<std::string>(), Eq("r_2"));
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, AfterRest) {
  constexpr auto argc = 5;
  std::array<const char *, argc> argv{
      "/usr/bin/test-program.exe", "r_1", "r_2", "r_3", "a_1"};

  auto default_command = std::make_shared<Command>(Command::DEFAULT);
  default_command->WithPositionals(MakeRest(), MakeAfter_1());

  Cli cli;
  cli.ProgramName("positionals").WithCommand(default_command);
  const auto &matches = cli.Parse(argc, argv.data());

  const auto &v_after_1 = matches.ValuesOf("AFTER_1");
  EXPECT_THAT(v_after_1.size(), Eq(1));
  EXPECT_THAT(v_after_1.at(0).GetAs<std::string>(), Eq("a_1"));

  const auto &v_rest = matches.ValuesOf(Option::key_rest);
  EXPECT_THAT(v_rest.size(), Eq(3));
  EXPECT_THAT(v_rest.at(0).GetAs<std::string>(), Eq("r_1"));
  EXPECT_THAT(v_rest.at(1).GetAs<std::string>(), Eq("r_2"));
  EXPECT_THAT(v_rest.at(2).GetAs<std::string>(), Eq("r_3"));
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, BeforeAndAfterRest) {
  constexpr auto argc = 5;
  std::array<const char *, argc> argv{
      "/usr/bin/test-program.exe", "b_1", "r_1", "r_2", "a_1"};

  auto default_command = std::make_shared<Command>(Command::DEFAULT);
  default_command->WithPositionals(MakeBefore_1(), MakeRest(), MakeAfter_1());

  Cli cli;
  cli.ProgramName("positionals").WithCommand(default_command);
  const auto &matches = cli.Parse(argc, argv.data());

  const auto &v_before_1 = matches.ValuesOf("BEFORE_1");
  EXPECT_THAT(v_before_1.size(), Eq(1));
  EXPECT_THAT(v_before_1.at(0).GetAs<std::string>(), Eq("b_1"));

  const auto &v_after_1 = matches.ValuesOf("AFTER_1");
  EXPECT_THAT(v_after_1.size(), Eq(1));
  EXPECT_THAT(v_after_1.at(0).GetAs<std::string>(), Eq("a_1"));

  const auto &v_rest = matches.ValuesOf(Option::key_rest);
  EXPECT_THAT(v_rest.size(), Eq(2));
  EXPECT_THAT(v_rest.at(0).GetAs<std::string>(), Eq("r_1"));
  EXPECT_THAT(v_rest.at(1).GetAs<std::string>(), Eq("r_2"));
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, UnexpectedPositionals) {
  constexpr auto argc = 2;
  std::array<const char *, argc> argv{"/usr/bin/test-program.exe", "pos"};

  auto default_command = std::make_shared<Command>(Command::DEFAULT);

  Cli cli;
  cli.ProgramName("positionals").WithCommand(default_command);
  try {
    cli.Parse(argc, argv.data());
    FAIL();
  } catch (...) {
  }
}

} // namespace

} // namespace asap::clap

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
