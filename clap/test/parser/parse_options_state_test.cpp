// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

#include "./test_helpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if defined(ASAP_IS_DEBUG_BUILD)
#include <contract/ut/gtest.h>
#endif

#include "clap/fluent/dsl.h"

using testing::IsTrue;

namespace asap::clap::parser::detail {

namespace {

class ParseOptionsStateTest : public StateTest {
public:
  [[maybe_unused]] static void SetUpTestSuite() {
    const Command::Ptr my_command{
        CommandBuilder("with-options")
            .WithOption(Option::WithKey("first_opt")
                            .About("The first option")
                            .Short("f")
                            .Long("first-option")
                            .WithValue<std::string>()
                            .DefaultValue("1")
                            .ImplicitValue("1")
                            .Build())
            .WithOption(Option::WithKey("second_opt")
                            .About("The second option")
                            .Short("s")
                            .Long("second-option")
                            .WithValue<unsigned>()
                            .DefaultValue(2)
                            .ImplicitValue(2)
                            .Build())
            .Build()};
    predefined_commands()["with-options"] = my_command;
  }

protected:
  void SetUp() override {
    StateTest::SetUp();
    state_ = std::make_unique<ParseOptionsState>();
  }

  [[nodiscard]] auto EnterState(
      const Token &token, const ParserContextPtr &context) const -> Status {
    ASAP_EXPECT(context->active_command);

    const auto &[token_type, token_value] = token;
    switch (token_type) {
    case TokenType::ShortOption: {
      const auto first_event = TokenEvent<TokenType::ShortOption>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::LongOption: {
      const auto first_event = TokenEvent<TokenType::LongOption>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::LoneDash: {
      const auto first_event = TokenEvent<TokenType::LoneDash>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::DashDash: {
      const auto first_event = TokenEvent<TokenType::DashDash>(token_value);
      return state_->OnEnter(first_event, context);
    }
    case TokenType::Value: {
      const auto first_event = TokenEvent<TokenType::Value>(token_value);
      return state_->OnEnter(first_event, context);
    }
      // The following token types are not allowed
    case TokenType::EqualSign:
    case TokenType::EndOfInput:
    default:;
    }
    return TerminateWithError{"Illegal token used to enter ParseOptionsState"};
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
  Command::Ptr command;
  const CommandLineContext base_context("test", command, ovm);
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
  const auto state = std::make_unique<ParseOptionsState>();
  const auto event = TokenEvent<TokenType::EndOfInput>("");
  OptionValuesMap ovm;
  Command::Ptr command;
  const CommandLineContext base_context("test", command, ovm);
  auto context = ParserContext::New(
      base_context, {StateTest::predefined_commands().at("default")});
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, context));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests, EnteringWithEmptyContextBreaksContract) {
  const auto state = std::make_unique<ParseOptionsState>();
  const auto event = TokenEvent<TokenType::Value>("xxx");
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, {}));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests,
    EnteringWithContextButNoActiveCommandBreaksContract) {
  const auto state = std::make_unique<ParseOptionsState>();
  const auto event = TokenEvent<TokenType::Value>("xxx");
  OptionValuesMap ovm;
  Command::Ptr command;
  const CommandLineContext base_context("test", command, ovm);
  auto context = ParserContext::New(
      base_context, {StateTest::predefined_commands().at("default")});
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event, {}));
}

// NOLINTNEXTLINE
TEST(ParseOptionsStateContractTests,
    EnteringWithNoContextButNoExistingContextBreaksContract) {
  const auto state = std::make_unique<ParseOptionsState>();
  const auto event = TokenEvent<TokenType::Value>("xxx");
  CHECK_VIOLATES_CONTRACT(state->OnEnter(event));
}
#endif // ASAP_IS_DEBUG_BUILD

} // namespace

} // namespace asap::clap::parser::detail
