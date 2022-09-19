# ===-----------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===-----------------------------------------------------------------------===#

message("=> [third-party modules]")

# Call `asap_add_package` to add third party packages using CPM, while
# automatically adding/removing them to the project hierarchy stack.
#
# Refer to https://github.com/cpm-cmake/CPM.cmake for the full usage.
#
# Example:
# ~~~
# asap_add_package(
#   NAME
#   magic_enum
#   GIT_TAG
#   master
#   GITHUB_REPOSITORY
#   abdes/magic_enum
#   OPTIONS
#   "MAGIC_ENUM_OPT_INSTALL ON")
# ~~~

# Live at Head philosophy. Always use the latest commit in the main branch in
# your projects.

# ------------------------------------------------------------------------------
# GSL
# ------------------------------------------------------------------------------

asap_add_package(NAME gsl GIT_TAG main GITHUB_REPOSITORY microsoft/GSL)

# ------------------------------------------------------------------------------
# magic_enum
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  magic_enum
  GIT_TAG
  master
  GITHUB_REPOSITORY
  abdes/magic_enum
  OPTIONS
  "MAGIC_ENUM_OPT_INSTALL ON")

# ------------------------------------------------------------------------------
# fmt::fmt-header-only
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  fmtlib
  GIT_TAG
  9.0.0
  GITHUB_REPOSITORY
  fmtlib/fmt
  OPTIONS
  "FMT_INSTALL ON")

# ------------------------------------------------------------------------------
# asap::common
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_common
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-common
  OPTIONS
  "ASAP_COMMON_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")

# ------------------------------------------------------------------------------
# asap::fsm
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_fsm
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-fsm
  OPTIONS
  "ASAP_FSM_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")

# ------------------------------------------------------------------------------
# asap::textwrap
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_textwrap
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-textwrap
  OPTIONS
  "ASAP_TEXTWRAP_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")

# ------------------------------------------------------------------------------
# asap::mixin
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_mixin
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-mixin
  OPTIONS
  "ASAP_MIXIN_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")

# ------------------------------------------------------------------------------
# asap::contract
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_contract
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-contract
  OPTIONS
  "ASAP_CONTRACT_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")

# ------------------------------------------------------------------------------
# asap::logging
# ------------------------------------------------------------------------------

asap_add_package(
  NAME
  asap_logging
  GIT_TAG
  master
  GITHUB_REPOSITORY
  asap-projects/asap-logging
  OPTIONS
  "ASAP_LOGGING_INSTALL ON"
  "ASAP_BUILD_TESTS OFF"
  "ASAP_BUILD_EXAMPLES OFF")
message(STATUS "${ASAP_LOG_PROJECT_HIERARCHY} >> third-party modules")
