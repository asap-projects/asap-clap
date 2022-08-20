//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <array>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "clap/cli.h"
#include "clap/command.h"
#include "clap/fluent/dsl.h"
#include "clap/option.h"

using ::testing::Eq;

namespace asap::clap {

namespace {

inline auto MakeBefore_1() -> std::shared_ptr<Option> {
  return Option::Positional("BEFORE_1")
      .About("first positional before rest")
      .WithValue<std::string>()
      .Build();
}

inline auto MakeBefore_2() -> std::shared_ptr<Option> {
  return Option::Positional("BEFORE_2")
      .About("second positional before rest")
      .WithValue<std::string>()
      .Build();
}

inline auto MakeRest() -> std::shared_ptr<Option> {
  return Option::Rest()
      .About("remaining positional arguments")
      .WithValue<std::string>()
      .Build();
}

inline auto MakeAfter_1() -> std::shared_ptr<Option> {
  return Option::Positional("AFTER_1")
      .About("first positional after rest")
      .WithValue<std::string>()
      .Build();
}

// NOLINTNEXTLINE
TEST(PositionalArgumentsTest, JustRest) {
  constexpr auto argc = 5;
  std::array<const char *, argc> argv{
      "/usr/bin/test-program.exe", "r_1", "r_2", "r_3", "r_4"};

  const auto default_command = std::make_shared<Command>(Command::DEFAULT);
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

  const auto default_command = std::make_shared<Command>(Command::DEFAULT);
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

  const auto default_command = std::make_shared<Command>(Command::DEFAULT);

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
