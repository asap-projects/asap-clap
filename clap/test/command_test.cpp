//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/command.h"

#include <common/compilers.h>
#include <exception>

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
using testing::IsTrue;

namespace asap::clap {

namespace {

// NOLINTNEXTLINE
TEST(Command, Default) {
  //! [Default command]
  class MyCommand : public Command {
  public:
    MyCommand() : Command(DEFAULT) {
    }
  };
  //! [Default command]
  const MyCommand cmd;
  EXPECT_THAT(cmd.Path().size(), Eq(1));
  EXPECT_THAT(cmd.Path(), testing::Contains(""));
  EXPECT_THAT(cmd.IsDefault(), IsTrue());
}

// NOLINTNEXTLINE
TEST(Command, OneSegmentPath) {
  //! [Non-default command path]
  class MyCommand : public Command {
  public:
    MyCommand() : Command("path") {
    }
  };
  //! [Non-default command path]
  const MyCommand cmd;
  EXPECT_THAT(cmd.Path().size(), Eq(1));
  EXPECT_THAT(cmd.Path(), testing::Contains("path"));
}

// NOLINTNEXTLINE
TEST(Command, MultiSegmentPath) {
  class MyCommand : public Command {
  public:
    MyCommand() : Command("segment1", "segment2") {
    }
  };
  const MyCommand cmd;
  EXPECT_THAT(cmd.Path().size(), Eq(2));
  EXPECT_THAT(cmd.Path(), testing::Contains("segment1"));
  EXPECT_THAT(cmd.Path(), testing::Contains("segment2"));
}

// NOLINTNEXTLINE
TEST(Command, DefaultFollowedByOtherSegmentIsIllegalPath) {
  class MyCommand : public Command {
  public:
    MyCommand() : Command("", "segment") {
    }
  };
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  ASSERT_THROW(MyCommand cmd, std::exception);
}

// NOLINTNEXTLINE
TEST(Command, MultipleSegmentsContainingDefaultIsIllegalPath) {
  class MyCommand : public Command {
  public:
    MyCommand() : Command("segment", "", "segment") {
    }
  };
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  ASSERT_THROW(MyCommand cmd, std::exception);
}

} // namespace

} // namespace asap::clap

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
