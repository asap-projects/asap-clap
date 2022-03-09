//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "./test_helpers.h"

#include <common/compilers.h>

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

using ::testing::Eq;
using ::testing::IsTrue;
using ::testing::StartsWith;

namespace asap::clap::parser::detail {

namespace {

class IdentifyCommandStateTest : public StateTest {
protected:
  void SetUp() override {
    StateTest::SetUp();
    state_ = std::make_unique<IdentifyCommandState>();
  }

  void EnterState(const Token &token, const ParserContextPtr &context) {
    const auto &[token_type, token_value] = token;
    EXPECT_THAT(token_type, Eq(TokenType::Value));
    auto first_event = TokenEvent<TokenType::Value>(token_value);
    state_->OnEnter(first_event, context);
  }

  void LeaveState() const override {
    auto last_event = TokenEvent<TokenType::EndOfInput>("");
    state_->OnLeave(last_event);
  }

  [[nodiscard]] auto state() const
      -> const std::unique_ptr<IdentifyCommandState> & {
    return state_;
  }

  void DoCheckStateAfterLastToken(const TestValueType &test_value) {
    const auto &[command_paths, args, action_check, state_check] = test_value;

    Tokenizer tokenizer(args);
    const auto commands = BuildCommands(command_paths);
    OptionValuesMap ovm;
    CommandLineContext base_context("test", ovm);
    auto context = ParserContext::New(base_context, commands);
    EnterState(tokenizer.NextToken(), context);
    while (true) {
      auto token = tokenizer.NextToken();
      if (!ProcessToken(token, state(), action_check, state_check)) {
        break;
      }
    }
  }

private:
  std::unique_ptr<IdentifyCommandState> state_;
};

class IdentifyCommandStateTransitionsTest
    : public IdentifyCommandStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(WellFormedScenarios,
    IdentifyCommandStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"just"},
            {"just"},
            FinalStateTransitionTestData{"just"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just"},
            {"just", "hello"},
            ParseOptionsTransitionTestData{"just", {}},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just"},
            {"just", "--hi"},
            ParseOptionsTransitionTestData{"just", {}},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just"},
            {"just", "--"},
            DashDashTransitionTestData{"just"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just", "just do"},
            {"just", "do"},
            FinalStateTransitionTestData{"just do"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just do", "just do it"},
            {"just", "do"},
            FinalStateTransitionTestData{"just do"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just", "just do it"},
            {"just", "do", "it"},
            FinalStateTransitionTestData{"just do it"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just", "just do it", "just do nothing"},
            {"just", "do", "it"},
            FinalStateTransitionTestData{"just do it"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"justice", "just"},
            {"just"},
            FinalStateTransitionTestData{"just"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"default", "just", "just do it", "just it"},
            {"just", "it"},
            FinalStateTransitionTestData{"just it"},
            IdentifyCommandStateTestData{}
        } // clang-format on
        ));

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(WellFormedScenariosWithOptions,
    IdentifyCommandStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType {
            {"just", "just do it"},
            {"just", "-f", "--test"},
            ParseOptionsTransitionTestData{"just", {}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"just", "just do it"},
            {"just", "do", "it", "-v"},
            ParseOptionsTransitionTestData{"just do it", {}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"just", "just do it"},
            {"just", "--", "it", "-v"},
            DashDashTransitionTestData{"just"},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"just do", "just do it"},
            {"just", "do", "something"},
            ParseOptionsTransitionTestData{"just do", {}},
            IdentifyCommandStateTestData{}
        } // clang-format on
        ));

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(BacktrackScenarios,
    IdentifyCommandStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType {
            {"default", "just do it"},
            {"just", "do", "--test"},
            ParseOptionsTransitionTestData{"default", {"just", "do"}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"default", "just do it"},
            {"just", "do", "--"},
            ParseOptionsTransitionTestData{"default", {"just", "do"}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"default", "just do it"},
            {"just", "do"},
            ParseOptionsTransitionTestData{"default", {"just", "do"}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"default", "just do it"},
            {"just"},
            ParseOptionsTransitionTestData{"default", {"just"}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"default", "just do it"},
            {"just", "--", "something"},
            ParseOptionsTransitionTestData{"default", {"just"}},
            IdentifyCommandStateTestData{}
        },
        TestValueType {
            {"default", "just do it"},
            {"just", "do", "something"},
            ParseOptionsTransitionTestData{"default", {"just", "do"}},
            IdentifyCommandStateTestData{}
        } // clang-format on
        ));

class IdentifyCommandStateErrorsTest
    : public IdentifyCommandStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(IllFormedScenarios, IdentifyCommandStateErrorsTest,
    // clang-format off
    ::testing::Values(
        TestValueType{
            {"just do", "just do it"},
            {"just", "not"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just do it"},
            {"just", "do", "-f"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"do it", "just do it", "just do nothing"},
            {"just", "do"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"justice", "just do it"},
            {"just", "-"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            IdentifyCommandStateTestData{}
        },
        TestValueType{
            {"just do it", "just it", "do it"},
            {"just", "do", "--it"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            IdentifyCommandStateTestData{}
        } // clang-format on
        ));

// NOLINTNEXTLINE
TEST_P(IdentifyCommandStateTransitionsTest, CheckStateAfterLastToken) {
  auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

// NOLINTNEXTLINE
TEST_P(IdentifyCommandStateErrorsTest, CheckStateAfterLastToken) {
  auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

// NOLINTNEXTLINE
TEST_F(IdentifyCommandStateTest, OnLeaveResetsTheState) {
  auto test_value = TestValueType{{"default", "just do it"}, {"just", "do"},
      ParseOptionsTransitionTestData{"default", {"just", "do"}},
      IdentifyCommandStateTestData{}};
  DoCheckStateAfterLastToken(test_value);
  DoCheckStateAfterLastToken(test_value);
}
} // namespace

} // namespace asap::clap::parser::detail
