# ===-----------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===-----------------------------------------------------------------------===#

message("=> [third-party modules]")

# Live at Head philosophy. Always use the latest commit in the main branch in
# your projects.

# ------------------------------------------------------------------------------
# GSL
# ------------------------------------------------------------------------------

cpmaddpackage(NAME gsl GIT_TAG main GITHUB_REPOSITORY microsoft/GSL)

# ------------------------------------------------------------------------------
# magic_enum
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  magic_enum
  GIT_TAG
  master
  GITHUB_REPOSITORY
  Neargye/magic_enum
  OPTIONS
  "MAGIC_ENUM_OPT_INSTALL ON")

# ------------------------------------------------------------------------------
# fmt::fmt-header-only
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  fmtlib
  GIT_TAG
  master
  GITHUB_REPOSITORY
  fmtlib/fmt
  OPTIONS
  "FMT_INSTALL ON")

# ------------------------------------------------------------------------------
# asap::common
# ------------------------------------------------------------------------------

cpmaddpackage(NAME asap_common GIT_TAG v2.0.4 GITHUB_REPOSITORY
              asap-projects/asap-common)

# ------------------------------------------------------------------------------
# asap::fsm
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  asap_fsm
  GIT_TAG
  v1.0.7
  GITHUB_REPOSITORY
  asap-projects/asap-fsm
  OPTIONS
  "ASAP_FSM_INSTALL ON")

# ------------------------------------------------------------------------------
# asap::textwrap
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  asap_textwrap
  GIT_TAG
  v1.0.4
  GITHUB_REPOSITORY
  asap-projects/asap-textwrap
  OPTIONS
  "ASAP_TEXTWRAP_INSTALL ON")

# ------------------------------------------------------------------------------
# asap::mixin
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  asap_mixin
  GIT_TAG
  v1.0.5
  GITHUB_REPOSITORY
  asap-projects/asap-mixin
  OPTIONS
  "ASAP_MIXIN_INSTALL ON")

# ------------------------------------------------------------------------------
# asap::contract
# ------------------------------------------------------------------------------

cpmaddpackage(
  NAME
  asap_contract
  GIT_TAG
  v1.0.1
  GITHUB_REPOSITORY
  asap-projects/asap-contract
  OPTIONS
  "ASAP_CONTRACT_INSTALL ON")
