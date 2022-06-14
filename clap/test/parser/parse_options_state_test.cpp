// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

#include "./test_helpers.h"

#include <common/compilers.h>
#if defined(ASAP_IS_DEBUG_BUILD)
#include <contract/ut/gtest.h>
#endif

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

class ParseOptionsStateTest : public StateTest {
public:
  static void SetUpTestSuite() {
    auto my_command = std::make_shared<Command>("with-options");
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
                               .WithValue<unsigned>()
                               .DefaultValue(2)
                               .ImplicitValue(2)
                               .Build());
    predefined_commands()["with-options"] = my_command;
  }

protected:
  void SetUp() override {
    StateTest::SetUp();
    state_ = std::make_unique<ParseOptionsState>();
  }

  auto EnterState(const Token &token, const ParserContextPtr &context)
      -> Status {
    ASAP_EXPECT(context->active_command);

    const auto &[token_type, token_value] = token;
    switch (token_type) {
    case TokenType::ShortOption: {
      auto first_event = TokenEvent<TokenType::ShortOption>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::LongOption: {
      auto first_event = TokenEvent<TokenType::LongOption>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::LoneDash: {
      auto first_event = TokenEvent<TokenType::LoneDash>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::DashDash: {
      auto first_event = TokenEvent<TokenType::DashDash>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::Value: {
      auto first_event = TokenEvent<TokenType::Value>(token_value);
      return state_->OnEnter(first_event, context);
    }
    default:
      return TerminateWithError{
          "Illegal token used to enter ParseOptionsState"};
    }
  }

  auto state() -> std::unique_ptr<ParseOptionsState> & {
    return state_;
  }

private:
  std::unique_ptr<ParseOptionsState> state_;
};

class ParseOptionsStateTransitionsTest
    : public ParseOptionsStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(WellFormedScenarios, ParseOptionsStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"with-options"},
            {"-f"},
            ParseShortOptionTransitionTestData{"with-options"},
            ParseOptionsStateTestData{{}}
        },
        TestValueType{
            {"with-options"},
            {"--first-option"},
            ParseLongOptionTransitionTestData{"with-options"},
            ParseOptionsStateTestData{{}}
        },
        TestValueType{
            {"with-options"},
            {"--not-an-option"},
            ParseLongOptionTransitionTestData{"with-options"},
            ParseOptionsStateTestData{{}}
        },
        TestValueType{
            {"with-options"},
            {"--"},
            DashDashTransitionTestData{"with-options"},
            ParseOptionsStateTestData{{}}
        },
        TestValueType{
            {"with-options"},
            {"-"},
            ParseShortOptionTransitionTestData{"with-options"},
            ParseOptionsStateTestData{{}}
        },
        TestValueType{
            {"with-options"},
            {"value"},
            DoNothingTransitionTestData{},
            ParseOptionsStateTestData{{"value"}}
        }
    )); // clang-format on

// NOLINTNEXTLINE
TEST_P(ParseOptionsStateTransitionsTest, CheckStateAfterLastToken) {
  auto test_value = GetParam();
  const auto &[command_paths, args, action_check, state_check] = test_value;

  Tokenizer tokenizer(args);
  const auto commands = BuildCommands(command_paths);
  OptionValuesMap ovm;
  CommandLineContext base_context("test", ovm);
  auto context = ParserContext::New(base_context, commands);
  context->active_command = predefined_commands().at("with-options");
  auto token = tokenizer.NextToken();
  auto status = EnterState(token, context);
  EXPECT_THAT(std::holds_alternative<ReissueEvent>(status), IsTrue());
  while (true) {
    token = tokenizer.NextToken();
    // We will never enter ParseOptionsState with an EndOfInput token, and all
    // test scenarios will have at least one token.
    if (token.first == TokenType::EndOfInput) {
      break;
    }
    if (!ProcessToken(token, state(), action_check, state_check)) {
      break;
    }
  }
}

// Contracts are not enforced in release builds
#if defined(ASAP_IS_DEBUG_BUILD)
// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests, EnteringWithEndOfInputBreaksContract) {
  auto state = std::make_unique<ParseOptionsState>();
  auto event = TokenEvent<TokenType::EndOfInput>("");
  OptionValuesMap ovm;
  CommandLineContext base_context("test", ovm);
  auto context = ParserContext::New(
      base_context, {StateTest::predefined_commands().at("default")});
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, context));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests, EnteringWithEmptyContextBreaksContract) {
  auto state = std::make_unique<ParseOptionsState>();
  auto event = TokenEvent<TokenType::Value>("xxx");
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, {}));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests,
    EnteringWithContextButNoActiveCommandBreaksContract) {
  auto state = std::make_unique<ParseOptionsState>();
  auto event = TokenEvent<TokenType::Value>("xxx");
  OptionValuesMap ovm;
  CommandLineContext base_context("test", ovm);
  auto context = ParserContext::New(
      base_context, {StateTest::predefined_commands().at("default")});
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, {}));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests,
    EnteringWithNoContextButNoExistingContextBreaksContract) {
  auto state = std::make_unique<ParseOptionsState>();
  auto event = TokenEvent<TokenType::Value>("xxx");
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event));
}
#endif // ASAP_IS_DEBUG_BUILD

} // namespace

} // namespace asap::clap::parser::detail
