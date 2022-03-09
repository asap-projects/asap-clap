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

class InitialStateTest : public StateTest {
protected:
  void SetupInitialState(ParserContextPtr &context) {
    state_ = std::make_unique<InitialState>(context);
  }

  [[nodiscard]] auto state() const -> const std::unique_ptr<InitialState> & {
    return state_;
  }

  void DoCheckStateAfterLastToken(const TestValueType &test_value) {
    const auto &[command_paths, args, action_check, state_check] = test_value;

    Tokenizer tokenizer(args);
    const auto commands = BuildCommands(command_paths);
    OptionValuesMap ovm;
    CommandLineContext base_context("test", ovm);
    auto context = ParserContext::New(base_context, commands);
    SetupInitialState(context);
    std::get<InitialStateTestData>(state_check).Check(state());
    while (true) {
      auto token = tokenizer.NextToken();
      if (!ProcessToken(token, state(), action_check, state_check)) {
        break;
      }
    }
  }

private:
  std::unique_ptr<InitialState> state_;
};

class InitialStateTransitionsTest
    : public InitialStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(WellFormedScenarios, InitialStateTransitionsTest,
    // clang-format off
    ::testing::Values(
        TestValueType {
            {"default"},
            {},
            FinalStateTransitionTestData{"default"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default")
            }}
        },
        TestValueType {
            {"default", "just"},
            {},
            FinalStateTransitionTestData{"default"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default"),
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"default"},
            {"--xx"},
            ParseOptionsTransitionTestData{"default", {}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default")
            }}
        },
        TestValueType {
            {"default"},
            {"--x"},
            ParseOptionsTransitionTestData{"default", {}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default")
            }}
        },
        TestValueType {
            {"default"},
            {"--"},
            DashDashTransitionTestData{"default"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default")
            }}
        },
        TestValueType {
            {"default"},
            {"-"},
            ParseOptionsTransitionTestData{"default", {}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default")
            }}
        },
        TestValueType {
            {"default", "partial"},
            {"part"},
            ParseOptionsTransitionTestData{"default", {}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default"),
              StateTest::predefined_commands().at("partial")
            }}
        },
        TestValueType {
            {"just"},
            {"just"},
            IdentifyCommandTransitionTestData{
              {StateTest::predefined_commands().at("just")}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"default", "just"},
            {"just"},
            IdentifyCommandTransitionTestData{{
              StateTest::predefined_commands().at("default"),
              StateTest::predefined_commands().at("just")}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("default"),
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"just do it"},
            {"just"},
            IdentifyCommandTransitionTestData{
              {StateTest::predefined_commands().at("just do it")}},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just do it")
            }}
        } // clang-format on
        ));

class InitialStateErrorsTest
    : public InitialStateTest,
      public ::testing::WithParamInterface<TestValueType> {};

// NOLINTNEXTLINE
INSTANTIATE_TEST_SUITE_P(IllFormedScenarios, InitialStateErrorsTest,
    // clang-format off
    ::testing::Values(
        TestValueType {
            {},
            {},
            ReportErrorTransitionTestData{"You must specify a command"},
            {}
        },
        TestValueType {
            {"just"},
            {},
            ReportErrorTransitionTestData{"You must specify a command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"just"},
            {"--xx"},
            ReportErrorTransitionTestData{"You must specify a command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"just"},
            {"-x"},
            ReportErrorTransitionTestData{"You must specify a command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"just"},
            {"--"},
            ReportErrorTransitionTestData{"You must specify a command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {"just"},
            {"-"},
            ReportErrorTransitionTestData{"You must specify a command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("just")
            }}
        },
        TestValueType {
            {},
            {"--x"},
            ReportErrorTransitionTestData{"You must specify a command"},
            {}
        },
        TestValueType {
            {},
            {"-x"},
            ReportErrorTransitionTestData{"You must specify a command"},
            {}
        },
        TestValueType {
            {},
            {"--"},
            ReportErrorTransitionTestData{"You must specify a command"},
            {}
        },
        TestValueType {
            {},
            {"-"},
            ReportErrorTransitionTestData{"You must specify a command"},
            {}
        },
        TestValueType {
            {"partial"},
            {"part"},
            ReportErrorTransitionTestData{"Unrecognized command"},
            InitialStateTestData{{
              StateTest::predefined_commands().at("partial")
            }}
        } // clang-format on
        ));

// NOLINTNEXTLINE
TEST_P(InitialStateTransitionsTest, CheckStateAfterLastToken) {
  auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

// NOLINTNEXTLINE
TEST_P(InitialStateErrorsTest, CheckStateAfterLastToken) {
  auto test_value = GetParam();
  DoCheckStateAfterLastToken(test_value);
}

} // namespace

} // namespace asap::clap::parser::detail
