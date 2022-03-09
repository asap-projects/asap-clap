# ===-----------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===-----------------------------------------------------------------------===#

message("=> [third-party modules]")

# Live at Head philosophy. Always use the latest commit in the main branch in
# your projects.

# ------------------------------------------------------------------------------
# magic_enum
# ------------------------------------------------------------------------------

FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG origin/main)
FetchContent_MakeAvailable(gsl)

# ------------------------------------------------------------------------------
# magic_enum
# ------------------------------------------------------------------------------

set(MAGIC_ENUM_OPT_INSTALL ON)
FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(magic_enum)

# ------------------------------------------------------------------------------
# fmt::fmt-header-only
# ------------------------------------------------------------------------------
set(FMT_INSTALL ON)
FetchContent_Declare(
  fmtlib
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(fmtlib)

# ------------------------------------------------------------------------------
# asap::common
# ------------------------------------------------------------------------------

FetchContent_Declare(
  asap_common
  GIT_REPOSITORY https://github.com/asap-projects/asap-common.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(asap_common)

# ------------------------------------------------------------------------------
# asap::fsm
# ------------------------------------------------------------------------------
set(ASAP_FSM_INSTALL ON)
FetchContent_Declare(
  asap_fsm
  GIT_REPOSITORY https://github.com/asap-projects/asap-fsm.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(asap_fsm)

# ------------------------------------------------------------------------------
# asap::textwrap
# ------------------------------------------------------------------------------
set(ASAP_TEXTWRAP_INSTALL ON)
FetchContent_Declare(
  asap_textwrap
  GIT_REPOSITORY https://github.com/asap-projects/asap-textwrap.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(asap_textwrap)

# ------------------------------------------------------------------------------
# asap::mixin
# ------------------------------------------------------------------------------
set(ASAP_MIXIN_INSTALL ON)
FetchContent_Declare(
  asap_mixin
  GIT_REPOSITORY https://github.com/asap-projects/asap-mixin.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(asap_mixin)

# ------------------------------------------------------------------------------
# asap::contract
# ------------------------------------------------------------------------------
set(ASAP_CONTRACT_INSTALL ON)
FetchContent_Declare(
  asap_contract
  GIT_REPOSITORY https://github.com/asap-projects/asap-contract.git
  GIT_TAG origin/master)
FetchContent_MakeAvailable(asap_contract)
