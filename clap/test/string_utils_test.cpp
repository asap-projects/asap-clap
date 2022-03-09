//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/detail/string_utils.h"

#include <common/compilers.h>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <optional>

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

namespace asap::clap::detail {

namespace {

// NOLINTNEXTLINE
TEST(StringUtils, ToLower) {
  EXPECT_THAT(
      detail::ToLower("True Enable DisABLe"), Eq("true enable disable"));
}

} // namespace

} // namespace asap::clap::detail

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
