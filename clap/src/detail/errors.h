// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

/*!
 * \file
 *
 * \brief Exceptions for the Command line arguments parser.
 */

#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include <fsm/fsm.h>

#include "../parser/context.h"
#include <clap/asap_clap_export.h>

namespace asap::clap::parser::detail {

ASAP_CLAP_API auto UnrecognizedCommand(
    const std::vector<std::string> &path_segments,
    const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto MissingCommand(const ParserContextPtr &context,
    const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto UnrecognizedOption(const ParserContextPtr &context,
    const std::string &token, const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto MissingValueForOption(const ParserContextPtr &context,
    const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto InvalidValueForOption(const ParserContextPtr &context,
    const std::string &token, const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto IllegalMultipleOccurrence(const ParserContextPtr &context,
    const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto OptionSyntaxError(const ParserContextPtr &context,
    const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto MissingRequiredOption(const CommandPtr &command,
    const OptionPtr &option, const char *message = nullptr) -> std::string;

ASAP_CLAP_API auto UnexpectedPositionalArguments(
    const ParserContextPtr &context, const char *message = nullptr)
    -> std::string;

} // namespace asap::clap::parser::detail
