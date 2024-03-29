//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/option_values_map.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
