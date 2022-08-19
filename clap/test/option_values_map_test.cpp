//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/option_values_map.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <common/compilers.h>

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

#include "parser/errors.h"

namespace asap::clap {

namespace {

// NOLINTNEXTLINE
TEST(OptionValuesMapTest, StoringAfterFinalThrows) {
  OptionValuesMap ovm;

  // TODO(Abdessattar): write unit tests for ovm

  ovm.StoreValue("verbose", {std::make_any<bool>(true), "true", false});
  ovm.StoreValue("verbose", {std::make_any<bool>(true), "true", false});
  ovm.StoreValue("verbose", {std::make_any<bool>(true), "true", false});
}

} // namespace

} // namespace asap::clap

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
