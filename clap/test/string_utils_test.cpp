//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/detail/string_utils.h"

#include <optional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
