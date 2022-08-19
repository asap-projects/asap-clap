//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/detail/args.h"

#include <algorithm>
#include <array>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/compilers.h>
#if defined(ASAP_IS_DEBUG_BUILD)
#include <contract/ut/gtest.h>
#endif

#include <gsl/gsl>

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
using testing::Ne;

namespace asap::clap::detail {

namespace {

// NOLINTNEXTLINE
TEST(ConstructArguments, WithNonEmptyProgramName) {
  constexpr auto argc = 1;
  std::array<const char *, 1> argv{"bin/test-program"};
  const Arguments cla{argc, argv.data()};
  EXPECT_THAT(cla.ProgramName(), Eq("bin/test-program"));
  EXPECT_THAT(cla.Args().size(), Eq(0));
}

// NOLINTNEXTLINE
TEST(ConstructArguments, WithManyArgs) {
  constexpr auto argc = 4;
  std::array<const char *, 4> argv{"test", "-x", "--opt=value", "arg"};
  const Arguments cla{argc, argv.data()};
  EXPECT_THAT(cla.ProgramName(), Eq("test"));
  EXPECT_THAT(cla.Args().size(), Eq(3));
  for (int index = 1; index < argc; index++) {
    EXPECT_THAT(
        std::find(cla.Args().begin(), cla.Args().end(), gsl::at(argv, index)),
        Ne(cla.Args().end()));
  }
}

#if defined(ASAP_IS_DEBUG_BUILD)
// NOLINTNEXTLINE
TEST(ConstructArguments, WithNoArgs) {
  constexpr auto argc = 0;
  std::array<const char *, 0> argv{};
  CHECK_VIOLATES_CONTRACT(auto args = Arguments(argc, argv.data()));
}

// NOLINTNEXTLINE
TEST(ConstructArguments, WithEmptyProgramName) {
  constexpr auto argc = 1;
  std::array<const char *, 1> argv{""};
  CHECK_VIOLATES_CONTRACT(auto args = Arguments(argc, argv.data()));
}
#endif // ASAP_IS_DEBUG_BUILD

} // namespace

} // namespace asap::clap::detail

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
