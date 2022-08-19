//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/with_usage.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/compilers.h>

#include "clap/command.h"

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

namespace asap::clap {

namespace {

// NOLINTNEXTLINE
TEST(WithUsage, AddUsageDetails) {
  class MyCommand : public Command,
                    asap::mixin::Mixin<MyCommand, WithUsageDetails> {
  public:
    MyCommand() : Command(DEFAULT), Mixin("Detailed usage help") {
    }

    [[nodiscard]] auto UsageDetails() const -> const std::string & {
      return usage_details;
    }
  };
  const MyCommand cmd;
  EXPECT_THAT(cmd.UsageDetails(), Eq("Detailed usage help"));
}

// NOLINTNEXTLINE
TEST(WithUsage, AddUsageExamples) {
  class MyCommand : public Command,
                    asap::mixin::Mixin<MyCommand, WithUsageExamples> {
  public:
    MyCommand() : Command(DEFAULT), Mixin("Usage examples") {
    }

    [[nodiscard]] auto UsageExamples() const -> const std::string & {
      return usage_examples;
    }
  };
  const MyCommand cmd;
  EXPECT_THAT(cmd.UsageExamples(), Eq("Usage examples"));
}

} // namespace

} // namespace asap::clap

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
