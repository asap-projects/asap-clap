//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <gmock/gmock.h>

#include <contract/ut/gtest.h>
#include <logging/logging.h>

auto main(int argc, char *argv[]) -> int {
  asap::logging::Registry::SetLogLevel(spdlog::level::off);

  asap::contract::PrepareForTesting();
  testing::InitGoogleTest(&argc, argv);
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
