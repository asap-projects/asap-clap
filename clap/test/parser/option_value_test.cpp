//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "clap/option_value.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "common/compilers.h"

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
using testing::IsFalse;
using testing::IsTrue;

namespace asap::clap::parser {

namespace {

// NOLINTNEXTLINE
TEST(OptionValuesTest, IsNotDefaultedWhenCreatedWithDefaultedFalse) {
  const std::any stored_value{123};
  const OptionValue value(stored_value, "123", false);
  EXPECT_THAT(value.IsDefaulted(), IsFalse());
  const OptionValue const_value(stored_value, "123", false);
  EXPECT_THAT(const_value.IsDefaulted(), IsFalse());
}

// NOLINTNEXTLINE
TEST(OptionValuesTest, IsDefaultedWhenCreatedWithDefaultedTrue) {
  const std::any stored_value{123};
  const OptionValue value(stored_value, "123", true);
  EXPECT_THAT(value.IsDefaulted(), IsTrue());
  const OptionValue const_value(stored_value, "123", true);
  EXPECT_THAT(const_value.IsDefaulted(), IsTrue());
}

// NOLINTNEXTLINE
TEST(OptionValuesTest, ReturnsOriginalToken) {
  const std::any stored_value{};
  const OptionValue value(stored_value, "123", false);
  EXPECT_THAT(value.OriginalToken(), Eq("123"));
  const OptionValue const_value(stored_value, "123", false);
  EXPECT_THAT(const_value.OriginalToken(), Eq("123"));
}

// NOLINTNEXTLINE
TEST(OptionValuesTest, ReturnsStoredValue) {
  const std::any stored_value{123};
  OptionValue opt_value(stored_value, "123", false);
  const std::any &value = opt_value.Value();
  EXPECT_THAT(std::any_cast<int>(value), Eq(std::any_cast<int>(stored_value)));
  const OptionValue const_opt_value(stored_value, "123", false);
  const std::any &const_value = const_opt_value.Value();
  EXPECT_THAT(
      std::any_cast<int>(const_value), Eq(std::any_cast<int>(stored_value)));
}

// NOLINTNEXTLINE
TEST(OptionValuesTest, ReturnsStoredValueWithCorrectType) {
  constexpr int stored_value{123};
  OptionValue opt_value(std::make_any<int>(stored_value), "123", false);
  const auto &value = opt_value.GetAs<int>();
  EXPECT_THAT(value, Eq(stored_value));
  const OptionValue const_opt_value(
      std::make_any<int>(stored_value), "123", false);
  const auto &const_value = const_opt_value.GetAs<int>();
  EXPECT_THAT(const_value, Eq(stored_value));
}

ASAP_DIAGNOSTIC_PUSH
#if defined(ASAP_MSVC_VERSION)
#pragma warning(disable : 4834)
#endif
// NOLINTNEXTLINE
TEST(OptionValuesTest, GetAsThrowsWithIncorrectType) {
  constexpr int stored_value{123};
  OptionValue opt_value(std::make_any<int>(stored_value), "123", false);
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  EXPECT_THROW([[maybe_unused]] auto &value = opt_value.GetAs<std::string>(),
      std::bad_any_cast);
  const OptionValue const_opt_value(
      std::make_any<int>(stored_value), "123", false);
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  EXPECT_THROW([[maybe_unused]] const auto &const_value =
                   const_opt_value.GetAs<std::string>(),
      std::bad_any_cast);
}

// NOLINTNEXTLINE
TEST(OptionValuesTest, GetAsThrowsWhenEmpty) {
  OptionValue opt_value(std::any{}, "", false);
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  EXPECT_THROW(
      [[maybe_unused]] auto &value = opt_value.GetAs<int>(), std::bad_any_cast);
  const OptionValue const_opt_value(std::any{}, "", false);
  // NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
  EXPECT_THROW(
      [[maybe_unused]] const auto &const_value = const_opt_value.GetAs<int>(),
      std::bad_any_cast);
}
ASAP_DIAGNOSTIC_POP

} // namespace

} // namespace asap::clap::parser

// NOLINTEND(used-but-marked-unused)
ASAP_DIAGNOSTIC_POP
