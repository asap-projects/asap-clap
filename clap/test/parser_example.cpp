//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/compilers.h>

#include "clap/fluent/dsl.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"

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

using testing::IsTrue;

namespace asap::clap::parser {

namespace {

// NOLINTNEXTLINE
TEST(ParserExample, ComplexCommandLine) {

  const auto common_options = std::make_shared<Options>("Common options");

  common_options->Add(Option::WithName("verbose")
                          .Short("v")
                          .Long("verbose")
                          .WithValue<bool>()
                          .Build());

  const auto default_command = std::make_shared<Command>(Command::DEFAULT);
  default_command->WithOptions(common_options);
  default_command->WithOption(Option::WithName("INPUT")
                                  .About("The input file")
                                  .WithValue<std::string>()
                                  .Build());

  const auto just_command = std::make_shared<Command>("just", "hello");
  just_command->WithOptions(common_options, /* hidden */ true);
  just_command->WithOption(Option::WithName("first_opt")
                               .About("The first option")
                               .Short("f")
                               .Long("first-option")
                               .WithValue<unsigned>()
                               .DefaultValue(1)
                               .ImplicitValue(1)
                               .Build());
  just_command->WithOption(Option::WithName("second_opt")
                               .About("The second option")
                               .Short("s")
                               .Long("second-option")
                               .WithValue<std::string>()
                               .DefaultValue("1")
                               .ImplicitValue("1")
                               .Build());

  const auto doit_command = std::make_shared<Command>("just", "do", "it");
  doit_command->WithOption(Option::WithName("third_opt")
                               .About("The third option")
                               .Short("t")
                               .Long("third-option")
                               .WithValue<unsigned>()
                               .Build());

  const std::vector<std::shared_ptr<Command>> commands{
      default_command, just_command, doit_command};

  // const Tokenizer tokenizer{{"just", "do", "it", "-t", "2222"}};
  const Tokenizer tokenizer{{
      "just", "hello", "-fv", "--second-option", "192.168.10.2:8080",
      //      "--allowed_ips=10.0.0.0/8,172.16.0.1/16",
      //        "--allowed_ids", "one,two", "now"
  }};
  OptionValuesMap ovm;
  const CommandLineContext context("parser-test", ovm);
  CmdLineParser parser(context, tokenizer, commands);
  const auto success = parser.Parse();
  ASSERT_THAT(success, IsTrue());
}

} // namespace

} // namespace asap::clap::parser
ASAP_DIAGNOSTIC_POP
// NOLINTEND(used-but-marked-unused)
