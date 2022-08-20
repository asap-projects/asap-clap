//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/with_usage.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "clap/command.h"

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
