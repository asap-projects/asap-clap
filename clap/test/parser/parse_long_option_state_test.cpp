// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

#include "./test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <contract/contract.h>

#include "clap/fluent/dsl.h"

namespace asap::clap::parser::detail {

namespace {

class ParseLongOptionStateTest : public StateTest {
public:
  [[maybe_unused]] static void SetUpTestSuite() {
    const auto my_command = std::make_shared<Command>("with-options");
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
    state_ = std::make_unique<ParseLongOptionState>();
  }

  void EnterState(const Token &token, const ParserContextPtr &context) const {
    ASAP_EXPECT(context->active_command);

    const auto &[token_type, token_value] = token;
    if (token_type == TokenType::LongOption) {
      const auto first_event = TokenEvent<TokenType::LongOption>(token_value);
      state_->OnEnter(first_event, context);
    } else {
      FAIL() << "Illegal token used to enter ParseLongOptionState: "
             << token_type << "/" << token_value << std::endl;
    }
  }

  void LeaveState() const override {
    const auto last_event = TokenEvent<TokenType::EndOfInput>("");
    state_->OnLeave(last_event);
  }

  auto state() -> std::unique_ptr<ParseLongOptionState> & {
    return state_;
  }
  void DoCheckStateAfterLastToken(const TestValueType &test_value) {
    const auto &[command_paths, args, action_check, state_check] = test_value;

    const Tokenizer tokenizer(args);
    const auto commands = BuildCommands(command_paths);
    const CommandLineContext base_context("test", ovm_);
    const auto context = ParserContext::New(base_context, commands);
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
  }

private:
  std::unique_ptr<ParseLongOptionState> state_;
  OptionValuesMap ovm_;
};

class ParseLongOptionStateTransitionsTest
    : public ParseLongOptionStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(OptionTakesNoValue,
    ParseLongOptionStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"--no-value", "2"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"opt_no_val", "--no-value", 1, {"true"}}
        }
    )); // clang-format on

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(OptionTakesOptionalValue,
    ParseLongOptionStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"--first-option"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"first_opt", "--no-value", 1, {"1"}}
        },
        TestValueType{
            {"with-options"},
            {"--first-option", "222"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"first_opt", "--no-value", 1, {"222"}}
        },
        TestValueType{
            {"with-options"},
            {"--first-option=333"},
            ParseOptionsTransitionTestData{},
            ParseShortOptionStateTestData{"first_opt", "--no-value", 1, {"333"}}
        }
    )); // clang-format on

// NOLINTNEXTLINE
TEST_P(ParseLongOptionStateTransitionsTest, TransitionWithNoError) {
  const auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

class ParseLongOptionStateUnrecognizedOptionTest
    : public ParseLongOptionStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(UnrecognizedOptions,
    ParseLongOptionStateUnrecognizedOptionTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"--not-option"},
            {},
            {}
        },
        TestValueType{
            {"with-options"},
            {"--second"},
            {},
            {}
        }
    )); // clang-format on

// NOLINTNEXTLINE
TEST_P(ParseLongOptionStateUnrecognizedOptionTest, FailWithAnException) {
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