// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

#include "./test_helpers.h"

#include <common/compilers.h>
#include <contract/contract.h>

#include <clap/fluent/dsl.h>

#include <gmock/gmock.h>
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

using testing::Eq;
using testing::IsFalse;
using testing::IsTrue;

namespace asap::clap::parser::detail {

namespace {

class ParseShortOptionStateTest : public StateTest {
public:
  static void SetUpTestSuite() {
    auto my_command = std::make_shared<Command>("with-options");
    my_command->WithOption(Option::WithName("opt_no_val")
                               .About("Option that takes no values")
                               .Short("n")
                               .Long("no-value")
                               .WithValue<bool>()
                               .DefaultValue(false, "false")
                               .ImplicitValue(true, "true")
                               .Build());
    my_command->WithOption(Option::WithName("first_opt")
                               .About("The first option")
                               .Short("f")
                               .Long("first-option")
                               .WithValue<std::string>()
                               .DefaultValue("1")
                               .ImplicitValue("1")
                               .Build());
    my_command->WithOption(Option::WithName("second_opt")
                               .About("The second option")
                               .Short("s")
                               .Long("second-option")
                               .WithValue<std::string>()
                               .Repeatable()
                               .Build());
    predefined_commands()["with-options"] = my_command;
  }

protected:
  void SetUp() override {
    StateTest::SetUp();
    state_ = std::make_unique<ParseShortOptionState>();
  }

  void EnterState(const Token &token, const ParserContextPtr &context) {
    ASAP_EXPECT(context->active_command);

    const auto &[token_type, token_value] = token;
    switch (token_type) {
    case TokenType::ShortOption: {
      auto first_event = TokenEvent<TokenType::ShortOption>(token_value);
      state_->OnEnter(first_event, context);
    } break;
    case TokenType::LoneDash: {
      auto first_event = TokenEvent<TokenType::LoneDash>(token_value);
      state_->OnEnter(first_event, context);
    } break;
    default:
      FAIL() << "Illegal token used to enter ParseShortOptionState: "
             << token_type << "/" << token_value << std::endl;
    }
  }

  void LeaveState() const override {
    auto last_event = TokenEvent<TokenType::EndOfInput>("");
    state_->OnLeave(last_event);
  }

  auto state() -> std::unique_ptr<ParseShortOptionState> & {
    return state_;
  }
  void DoCheckStateAfterLastToken(const TestValueType &test_value) {
    const auto &[command_paths, args, action_check, state_check] = test_value;

    Tokenizer tokenizer(args);
    const auto commands = BuildCommands(command_paths);
    CommandLineContext base_context("test", ovm_);
    auto context = ParserContext::New(base_context, commands);
    context->active_command = predefined_commands().at("with-options");
    auto token = tokenizer.NextToken();
    // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
    EXPECT_NO_THROW(EnterState(token, context));
    while (true) {
      token = tokenizer.NextToken();
      if (!ProcessToken(token, state(), action_check, state_check)) {
        break;
      }
    }
    LeaveState();
  }

private:
  std::unique_ptr<ParseShortOptionState> state_;
  OptionValuesMap ovm_;
};

class ParseShortOptionStateTransitionsTest
    : public ParseShortOptionStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(OptionTakesNoValue,
    ParseShortOptionStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"-n"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"opt_no_val", "-n", 1, {"true"}}
        }
    )); // clang-format on

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(OptionTakesOptionalValue,
    ParseShortOptionStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"-f"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"first_opt", "-f", 1, {"1"}}
        },
        TestValueType{
            {"with-options"},
            {"-f", "2"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"first_opt", "-f", 1, {"2"}}
        }
    )); // clang-format on

// NOLINTNEXTLINE
TEST_P(ParseShortOptionStateTransitionsTest, TransitionWithNoError) {
  auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

class ParseShortOptionStateUnrecognizedOptionTest
    : public ParseShortOptionStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(UnrecognizedOptions,
    ParseShortOptionStateUnrecognizedOptionTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"-d"},
            {},
            {}
        },
        TestValueType{
            {"with-options"},
            {"-df", "2"},
            {},
            {}
        },
        TestValueType{
            {"with-options"},
            {"-"},
            {},
            {}
        }
    )); // clang-format on

// NOLINTNEXTLINE
TEST_P(ParseShortOptionStateUnrecognizedOptionTest, FailWithAnException) {
  auto test_value = GetParam();
  const auto &[command_paths, args, action_check, state_check] = test_value;

  Tokenizer tokenizer(args);
  const auto commands = BuildCommands(command_paths);
  OptionValuesMap ovm;
  CommandLineContext base_context("test", ovm);
  auto context = ParserContext::New(base_context, commands);
  context->active_command = predefined_commands().at("with-options");
  auto token = tokenizer.NextToken();
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  EXPECT_THROW(EnterState(token, context), UnrecognizedOption);
}

} // namespace

} // namespace asap::clap::parser::detail
