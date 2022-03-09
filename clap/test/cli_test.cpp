//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/cli.h"
#include "clap/command.h"
#include "clap/option.h"
#include "clap/with_usage.h"

#include <common/compilers.h>
#include <mixin/mixin.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
#include <fmt/core.h>
ASAP_DIAGNOSTIC_POP

#include <array>
#include <memory>

#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest.h>

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

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
class BaseCli {
public:
  [[nodiscard]] virtual auto CommandLine() -> Cli & = 0;

  [[nodiscard]] auto CommonOptions() const -> const std::shared_ptr<Options> & {
    if (!common_options_) {
      common_options_ = std::make_shared<Options>("Common options");
      common_options_->Add(Option::WithName("help")
                               .Long("help")
                               .About("show this message, then exit")
                               .WithValue(ValueDescriptor<bool>::Create()));
      common_options_->Add(
          Option::WithName("version")
              .About(
                  fmt::format("show {} version info, then exit", ProgramName()))
              .Long("version")
              .WithValue(ValueDescriptor<bool>::Create()));
    }
    return common_options_;
  }

protected:
  ~BaseCli() = default;

  [[nodiscard]] virtual auto ProgramName() const -> const char * = 0;

private:
  mutable Options::Ptr common_options_;
};

constexpr const auto about_head =
    "Print the first 10 lines of each FILE to standard output. With more than "
    "one FILE, precede each with a header giving the file name.\n"
    "\n"
    "With no FILE, or when FILE is -, read standard input.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.";

constexpr const auto head_command_detailed_usage =
    "NUM may have a multiplier suffix: b 512, kB 1000, K 1024, MB  1000*1000, "
    "M 1024*1024, GB 1000*1000*1000, G 1024*1024*1024, and so on for T, P, E, "
    "Z, Y. Binary prefixes can be used, too: KiB=K, MiB=M, and so on.";

constexpr const auto usage_footer =
    "GNU coreutils online help: <https://www.gnu.org/software/coreutils/>. "
    "Report head translation bugs to <https://translationproject.org/team/>. "
    "Full documentation at: <https://www.gnu.org/software/coreutils/head> or "
    "available locally via: info '(coreutils) head invocation'.";

constexpr const int default_num_lines = 10;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
class HeadCli : public BaseCli {
public:
  class MyCommand : public Command,
                    asap::mixin::Mixin<MyCommand, WithUsageDetails> {
  public:
    explicit MyCommand(std::string name)
        : Command(std::move(name)), Mixin(head_command_detailed_usage) {
    }
  };

  virtual ~HeadCli() = default;
  auto CommandLine() -> Cli & override {
    if (!cli_) {
      cli_.emplace();
      auto command = MakeCommand(Command::DEFAULT);
      command->WithOptions(CommonOptions());
      cli_->ProgramName(ProgramName())
          .Version("1.1.0")
          .About(about_head)
          // TODO(Abdessattar) support usage footer
          //.Footer(usage_footer)
          .WithCommand(command);
    }
    return cli_.value();
  }
  [[nodiscard]] auto MakeCommand(std::string name) const -> Command::Ptr {
    if (!command_) {
      command_ = std::make_shared<HeadCli::MyCommand>(std::move(name));
      command_->About("output the first part of files");
      command_->WithOption(
          Option::WithName("bytes")
              .About("print the first NUM bytes of each file; with the leading "
                     "'-', print all but the last NUM bytes of each file")
              .Short("c")
              .Long("bytes")
              .WithValue(ValueDescriptor<int>::Create()));
      command_->WithOption(
          Option::WithName("lines")
              .About("print the first NUM lines instead of the first 10; with "
                     "the leading '-', print all but the last  NUM lines of "
                     "each file")
              .Short("n")
              .Long("lines")
              .WithValue(ValueDescriptor<int>::Create().DefaultValue(
                  default_num_lines)));
      command_->WithOption(Option::WithName("quiet")
                               .About("never print headers giving file names")
                               .Short("q")
                               .Long("quiet")
                               // TODO(Abdessattar): support multiple name
                               // aliases .Long("silent")
                               .WithValue(ValueDescriptor<bool>::Create()));
      command_->WithOption(Option::WithName("verbose")
                               .About("always print headers giving file names")
                               .Short("v")
                               .Long("verbose")
                               .WithValue(ValueDescriptor<bool>::Create()));
      command_->WithOption(Option::WithName("verbose")
                               .About("line delimiter is NUL, not newline")
                               .Short("z")
                               .Long("zero-terminated")
                               .WithValue(ValueDescriptor<bool>::Create()));
      command_->WithPositionals(
          Option::Rest().WithValue(ValueDescriptor<std::string>::Create()));
    }
    return command_;
  }

protected:
  [[nodiscard]] auto ProgramName() const -> const char * override {
    return "head";
  }

private:
  mutable Command::Ptr command_;
  std::optional<Cli> cli_;
};

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
class PaintCli : public BaseCli {
public:
  enum class Color { red = 1, blue = 2, green = 3 };

  virtual ~PaintCli() = default;
  auto CommandLine() -> Cli & override {
    if (!cli_) {
      cli_.emplace();
      auto command = MakeCommand(Command::DEFAULT);
      command->WithOptions(CommonOptions());
      cli_->ProgramName(ProgramName())
          .Version("1.0.0")
          .About("Paint something using a color")
          // TODO(Abdessattar) support usage footer
          //.Footer(usage_footer)
          .WithCommand(command);
    }
    return cli_.value();
  }
  [[nodiscard]] auto MakeCommand(std::string name) const -> Command::Ptr {
    if (!command_) {
      command_ = std::make_shared<Command>(std::move(name));
      command_->About("paint something");
      command_->WithOption(
          Option::WithName("color")
              .About("select a color from possible values `Red`(1), `Green`(2) "
                     "or `Blue`(3)")
              .Short("c")
              .Long("color")
              .WithValue(
                  ValueDescriptor<Color>::Create().Required().Repeatable()));
    }
    return command_;
  }

protected:
  [[nodiscard]] auto ProgramName() const -> const char * override {
    return "paint";
  }

private:
  mutable Command::Ptr command_;
  std::optional<Cli> cli_;
};

class UtilsCli : public HeadCli {
public:
  auto CommandLine() -> Cli & override {
    if (!cli_) {
      cli_.emplace();
      auto default_command = std::make_shared<Command>(Command::DEFAULT);
      default_command->WithOptions(CommonOptions());
      HeadCli head;
      PaintCli paint;
      cli_->ProgramName(ProgramName())
          .Version("1.1.0")
          .About("GNU Core Utils - the basic file, shell and text manipulation "
                 "utilities of the GNU operating system.")
          // TODO(Abdessattar) support usage footer
          //.Footer(usage_footer)
          .WithCommand(default_command)
          .WithCommand(head.MakeCommand("head"))
          .WithCommand(paint.MakeCommand("paint"));
    }
    return cli_.value();
  }

protected:
  [[nodiscard]] auto ProgramName() const -> const char * override {
    return "utils";
  }

private:
  std::optional<Cli> cli_;
};

// NOLINTNEXTLINE
TEST(CommandLineTest, Test) {
  {
    constexpr size_t argc = 5;
    std::array<const char *, argc> argv{
        "/usr/bin/test-program.exe", "head", "--lines=+20", "-q", "file.txt"};

    UtilsCli cli;
    cli.CommandLine().Print(std::cout, 80);
    const auto &matches = cli.CommandLine().Parse(argc, argv.data());

    const auto &v_lines = matches.ValuesOf(("lines"));
    EXPECT_THAT(v_lines.size(), Eq(1));
    EXPECT_THAT(v_lines.at(0).GetAs<int>(), Eq(20));

    const auto &v_quiet = matches.ValuesOf(("quiet"));
    EXPECT_THAT(v_quiet.size(), Eq(1));
    EXPECT_THAT(v_quiet.at(0).GetAs<bool>(), Eq(true));

    const auto &v_rest = matches.ValuesOf(Option::key_rest);
    EXPECT_THAT(v_rest.size(), Eq(1));
    EXPECT_THAT(v_rest.at(0).GetAs<std::string>(), Eq("file.txt"));
  }
  {
    constexpr size_t argc = 2;
    std::array<const char *, argc> argv{
        "/usr/bin/test-program.exe", "--version"};

    HeadCli cli;
    const auto &matches = cli.CommandLine().Parse(argc, argv.data());
    const auto &values = matches.ValuesOf(("version"));
    EXPECT_THAT(values.size(), Eq(1));
    EXPECT_THAT(values.at(0).GetAs<bool>(), testing::IsTrue());
  }
  {
    constexpr size_t argc = 2;
    std::array<const char *, argc> argv{
        "/usr/bin/test-program.exe", "--version"};

    UtilsCli cli;
    const auto &matches = cli.CommandLine().Parse(argc, argv.data());
    const auto &values = matches.ValuesOf(("version"));
    EXPECT_THAT(values.size(), Eq(1));
    EXPECT_THAT(values.at(0).GetAs<bool>(), testing::IsTrue());
  }
  {
    constexpr size_t argc = 8;
    std::array<const char *, argc> argv{"/usr/bin/test-program.exe", "paint",
        "-c", "red", "--color=GREEN", "--color=bLue", "--color=1", "--color=3"};

    UtilsCli cli;
    const auto &matches = cli.CommandLine().Parse(argc, argv.data());
    const auto &values = matches.ValuesOf(("color"));
    EXPECT_THAT(values.size(), Eq(5));
    EXPECT_THAT(
        values.at(0).GetAs<PaintCli::Color>(), Eq(PaintCli::Color::red));
    EXPECT_THAT(
        values.at(1).GetAs<PaintCli::Color>(), Eq(PaintCli::Color::green));
  }
}

} // namespace

} // namespace asap::clap

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
