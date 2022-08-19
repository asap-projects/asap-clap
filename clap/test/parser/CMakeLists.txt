# ===------------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===------------------------------------------------------------------------===#

# ==============================================================================
# Build instructions
# ==============================================================================

set(MAIN_TEST_TARGET_NAME ${MODULE_TARGET_NAME}_parser_test)

asap_add_test(
  ${MAIN_TEST_TARGET_NAME}
  UNIT_TEST
  VALGRIND_MEMCHECK
  SRCS
  "test_helpers.h"
  "test_helpers.cpp"
  "tokenizer_test.cpp"
  "initial_state_test.cpp"
  "identify_command_state_test.cpp"
  "parse_options_state_test.cpp"
  "parse_short_option_state_test.cpp"
  "parse_long_option_state_test.cpp"
  "option_value_test.cpp"
  "../main.cpp"
  LINK
  asap::common
  asap::logging
  asap::contract-ut
  asap::clap
  fmt::fmt-header-only
  gtest
  gmock
  COMMENT
  "ASAP clap parser unit tests")

target_include_directories(${MAIN_TEST_TARGET_NAME} PRIVATE "../../src")

gtest_discover_tests(${MAIN_TEST_TARGET_NAME})

# Add support for (optional) code quality tools
asap_add_sanitizers(${MAIN_TEST_TARGET_NAME})
swift_add_valgrind_massif(${MAIN_TEST_TARGET_NAME})
swift_add_valgrind_callgrind(${MAIN_TEST_TARGET_NAME})
