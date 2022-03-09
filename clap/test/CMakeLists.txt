# ===------------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===------------------------------------------------------------------------===#

# ==============================================================================
# Build instructions
# ==============================================================================

set(MAIN_TEST_TARGET_NAME ${MODULE_TARGET_NAME}_test)

asap_add_test(
  ${MAIN_TEST_TARGET_NAME}
  UNIT_TEST
  VALGRIND_MEMCHECK
  SRCS
  "arguments_test.cpp"
  "cli_test.cpp"
  "command_test.cpp"
  "option_values_map_test.cpp"
  "parse_value_test.cpp"
  "parser_example.cpp"
  "string_utils_test.cpp"
  "with_usage_test.cpp"
  "main.cpp"
  LINK
  asap::common
  asap::contract-ut
  asap::clap
  GSL
  gtest
  gmock
  COMMENT
  "ASAP clap unit tests")

target_include_directories(${MAIN_TEST_TARGET_NAME} PRIVATE "../src")

gtest_discover_tests(${MAIN_TEST_TARGET_NAME})

# Add support for (optional) code quality tools
asap_add_sanitizers(${MAIN_TEST_TARGET_NAME})
swift_add_valgrind_massif(${MAIN_TEST_TARGET_NAME})
swift_add_valgrind_callgrind(${MAIN_TEST_TARGET_NAME})

add_subdirectory(parser)
add_subdirectory(positionals)
