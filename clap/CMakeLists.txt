# ===------------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===------------------------------------------------------------------------===#

set(my_name "clap")
asap_push_module("${my_name}")

# ------------------------------------------------------------------------------
# Meta information about the this module
# ------------------------------------------------------------------------------

asap_declare_module(
  MODULE_NAME
  "${my_name}"
  DESCRIPTION
  "Command line arguments parser."
  GITHUB_REPO
  "https://github.com/abdes/asap"
  AUTHOR_MAINTAINER
  "Abdessattar Sassi"
  VERSION_MAJOR
  "1"
  VERSION_MINOR
  "0"
  VERSION_PATCH
  "0")

# ==============================================================================
# Build instructions
# ==============================================================================

# ------------------------------------------------------------------------------
# Main module target
# ------------------------------------------------------------------------------

# Hardcode `asap` in the module name as we do not want this prefix to change
# with the forked project # name.
set(MODULE_TARGET_NAME "asap_${META_MODULE_NAME}")

asap_add_library(
  ${MODULE_TARGET_NAME}
  STATIC
  SHARED
  WARNING
  SOURCES
  # Headers
  "include/clap/cli.h"
  "include/clap/command.h"
  "include/clap/command_line_context.h"
  "include/clap/detail/args.h"
  "include/clap/detail/parse_value.h"
  "include/clap/detail/string_utils.h"
  "include/clap/detail/value_descriptor.h"
  "include/clap/fluent/dsl.h"
  "include/clap/fluent/option_builder.h"
  "include/clap/fluent/option_value_builder.h"
  "include/clap/fluent/positional_option_builder.h"
  "include/clap/option.h"
  "include/clap/option_value.h"
  "include/clap/option_values_map.h"
  "include/clap/value_semantics.h"
  "include/clap/with_usage.h"
  # Sources
  "src/cli.cpp"
  "src/command.cpp"
  "src/detail/args.cpp"
  "src/detail/errors.cpp"
  "src/detail/errors.h"
  "src/fluent/option_builder.cpp"
  "src/option.cpp"
  "src/parser/context.h"
  "src/parser/events.h"
  "src/parser/parser.cpp"
  "src/parser/parser.h"
  "src/parser/states.h"
  "src/parser/tokenizer.cpp"
  "src/parser/tokenizer.h")

target_link_libraries(
  ${MODULE_TARGET_NAME}
  PRIVATE asap::common asap::logging GSL
  PUBLIC magic_enum::magic_enum fmt::fmt asap::fsm asap::textwrap asap::mixin
         asap::contract)

target_include_directories(
  ${MODULE_TARGET_NAME}
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(${MODULE_TARGET_NAME} PUBLIC cxx_std_17)

# logging comes directly from asap and is usually kept as is. It is more
# convenient to simply use it as asap::logging
add_library(asap::${META_MODULE_NAME} ALIAS ${MODULE_TARGET_NAME})

# Generate module config files for cmake and pkgconfig
asap_create_module_config_files()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
if(ASAP_BUILD_TESTS)
  asap_add_sanitizers(${MODULE_TARGET_NAME})
  add_subdirectory(test)
endif()

# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
if(ASAP_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# ------------------------------------------------------------------------------
# API Documentation
# ------------------------------------------------------------------------------
asap_with_doxygen(
  MODULE_NAME
  ${MODULE_TARGET_NAME}
  VERSION
  ${META_MODULE_VERSION}
  TITLE
  "\"Command Line Arguments Parser\""
  BRIEF
  "\"Command line argument parsing and validation library.\""
  INPUT_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include")

asap_with_sphinx(${MODULE_TARGET_NAME})

# ==============================================================================
# Deployment instructions
# ==============================================================================
if(${META_PROJECT_ID}_INSTALL)
  set(TARGETS_EXPORT_NAME "${MODULE_TARGET_NAME}Targets")
  set(runtime "${MODULE_TARGET_NAME}_runtime")
  set(dev "${MODULE_TARGET_NAME}_dev")

  # Library
  install(
    TARGETS ${MODULE_TARGET_NAME}
    EXPORT "${TARGETS_EXPORT_NAME}"
    COMPONENT dev
    RUNTIME DESTINATION ${ASAP_INSTALL_BIN} COMPONENT ${runtime}
    LIBRARY DESTINATION ${ASAP_INSTALL_SHARED} COMPONENT ${runtime}
    ARCHIVE DESTINATION ${ASAP_INSTALL_LIB} COMPONENT ${dev})

  # Header files
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/${META_MODULE_NAME}
    DESTINATION ${ASAP_INSTALL_INCLUDE}
    COMPONENT ${dev}
    FILES_MATCHING
    PATTERN "*.h")

  # Generated header files
  install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/${META_MODULE_NAME}
    DESTINATION ${ASAP_INSTALL_INCLUDE}
    COMPONENT ${dev})

  # Target config
  install(
    EXPORT ${TARGETS_EXPORT_NAME}
    NAMESPACE ${META_PROJECT_NAME}::
    DESTINATION ${ASAP_INSTALL_CMAKE}/${META_MODULE_NAME}
    COMPONENT ${dev})

  # Package configuration files
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}Config.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_TARGET_NAME}ConfigVersion.cmake
    DESTINATION ${ASAP_INSTALL_CMAKE}/${META_MODULE_NAME})

  # Docs
  if(EXISTS ${SPHINX_BUILD_DIR}/${MODULE_TARGET_NAME})
    install(
      DIRECTORY ${SPHINX_BUILD_DIR}/${MODULE_TARGET_NAME}
      DESTINATION ${ASAP_INSTALL_DOC}
      COMPONENT ${MODULE_TARGET_NAME}_docs)
  endif()
endif()

asap_pop_module("${my_name}")
