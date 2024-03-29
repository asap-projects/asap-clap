// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

/*!
 * \file
 *
 * \brief Implementation details of the command line parser errors.
 */

#include "errors.h"

#include <numeric>

#include <common/compilers.h>
#include <contract/contract.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(ASAP_CLANG_VERSION)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif
#if defined(ASAP_GNUC_VERSION)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include <fmt/core.h>
#include <fmt/format.h>
ASAP_DIAGNOSTIC_POP

namespace {
void AppendOptionalMessage(std::string &description, const char *message) {
  if (message == nullptr || strlen(message) == 0) {
    description.append(".");
    return;
  }
  description.append(" - ").append(message).append(".");
}

auto CommandDiagnostic(const asap::clap::parser::detail::CommandPtr &command)
    -> std::string {
  if (!command || command->IsDefault()) {
    return "";
  }
  return fmt::format("while parsing command '{}',", command->PathAsString());
}

} // namespace

auto asap::clap::parser::detail::UnrecognizedCommand(
    const std::vector<std::string> &path_segments, const char *message)
    -> std::string {
  auto description = fmt::format(
      "Unrecognized command with path '{}'", fmt::join(path_segments, " "));
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::MissingCommand(
    const ParserContextPtr &context, const char *message) -> std::string {
  auto supported_commands =
      std::accumulate(context->commands.cbegin(), context->commands.cend(),
          std::vector<std::string>(), [](auto &dest, const auto &command) {
            dest.push_back("'" + command->PathAsString() + "'");
            return dest;
          });

  auto description =
      fmt::format("You must specify a command. Supported commands are: {}",
          fmt::join(supported_commands, ", "));
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::UnrecognizedOption(
    const ParserContextPtr &context, const std::string &token,
    const char *message) -> std::string {

  auto option_name = (token.length() == 1) ? "-" + token : "--" + token;
  auto description = fmt::format("{} '{}' is not a recognized option",
      CommandDiagnostic(context->active_command), option_name);
  AppendOptionalMessage(description, message);
  return description;
}
auto asap::clap::parser::detail::IllegalMultipleOccurrence(
    const ParserContextPtr &context, const char *message) -> std::string {
  ASAP_EXPECT(context->active_option);
  ASAP_EXPECT(context->ovm.OccurrencesOf(context->active_option->Key()) > 0);

  const auto &option_name = context->active_option->Key();
  auto description =
      fmt::format("{} new occurrence for option '{}' "
                  "as '{}' is illegal; it can only be used one time and it "
                  "appeared before with value '{}'",
          CommandDiagnostic(context->active_command), option_name,
          context->active_option_flag,
          context->ovm.ValuesOf(option_name).front().OriginalToken());
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::OptionSyntaxError(
    const ParserContextPtr &context, const char *message) -> std::string {
  auto description = fmt::format("{} option '{}' is using an invalid syntax",
      CommandDiagnostic(context->active_command),
      context->active_option->Key());
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::MissingValueForOption(
    const ParserContextPtr &context, const char *message) -> std::string {
  auto description =
      fmt::format("{} option '{}' seen as '{}' "
                  "has no value on the command line and no implicit one",
          CommandDiagnostic(context->active_command),
          context->active_option->Key(), context->active_option_flag);
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::InvalidValueForOption(
    const ParserContextPtr &context, const std::string &token,
    const char *message) -> std::string {

  auto description = fmt::format(
      "{} option '{}' seen as '{}',"
      " got value token '{}' which failed to parse to type {},"
      " and the option has no implicit value",
      CommandDiagnostic(context->active_command), context->active_option->Key(),
      context->active_option_flag, token, "<TODO: TYPE NAME>");
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::MissingRequiredOption(
    const CommandPtr &command, const OptionPtr &option, const char *message)
    -> std::string {
  auto description =
      fmt::format("{} no {} '{}' was specified. "
                  "It is required and does not have a default value",
          CommandDiagnostic(command),
          (option->IsPositional() ? "positional argument" : "option"),
          option->UserFriendlyName());
  AppendOptionalMessage(description, message);
  return description;
}

auto asap::clap::parser::detail::UnexpectedPositionalArguments(
    const ParserContextPtr &context, const char *message) -> std::string {
  auto description = fmt::format("{} argument{} '{}' "
                                 "{} not expected by any option",
      CommandDiagnostic(context->active_command),
      context->positional_tokens.size() > 1 ? "s" : "",
      fmt::join(context->positional_tokens, ", "),
      context->positional_tokens.size() > 1 ? "are" : "is");
  AppendOptionalMessage(description, message);
  return description;
}
