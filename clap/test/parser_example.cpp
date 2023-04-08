//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "clap/fluent/dsl.h"
#include "parser/parser.h"

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

  const Command::Ptr default_command{
      CommandBuilder(Command::DEFAULT)
          .WithOptions(common_options)
          .WithOption(Option::WithName("INPUT")
                          .About("The input file")
                          .WithValue<std::string>()
                          .Build())
          .Build()};

  const Command::Ptr just_command{
      CommandBuilder("just", "hello")
          .WithOptions(common_options, /* hidden */ true)
          .WithOption(Option::WithName("first_opt")
                          .About("The first option")
                          .Short("f")
                          .Long("first-option")
                          .WithValue<unsigned>()
                          .DefaultValue(1)
                          .ImplicitValue(1)
                          .Build())
          .WithOption(Option::WithName("second_opt")
                          .About("The second option")
                          .Short("s")
                          .Long("second-option")
                          .WithValue<std::string>()
                          .DefaultValue("1")
                          .ImplicitValue("1")
                          .Build())
          .Build()};

  const Command::Ptr doit_command{CommandBuilder("just", "do", "it")
                                      .WithOption(Option::WithName("third_opt")
                                                      .About("The third option")
                                                      .Short("t")
                                                      .Long("third-option")
                                                      .WithValue<unsigned>()
                                                      .Build())
                                      .Build()};

  const std::vector<std::shared_ptr<Command>> commands{
      default_command, just_command, doit_command};

  // const Tokenizer tokenizer{{"just", "do", "it", "-t", "2222"}};
  const Tokenizer tokenizer{{
      "just", "hello", "-fv", "--second-option", "192.168.10.2:8080",
      //      "--allowed_ips=10.0.0.0/8,172.16.0.1/16",
      //        "--allowed_ids", "one,two", "now"
  }};
  OptionValuesMap ovm;
  Command::Ptr command;
  const CommandLineContext context("parser-test", command, ovm);
  CmdLineParser parser(context, tokenizer, commands);
  const auto success = parser.Parse();
  ASSERT_THAT(success, IsTrue());
}

} // namespace

} // namespace asap::clap::parser
