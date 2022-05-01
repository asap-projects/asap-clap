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

#include <common/compilers.h>
#include <contract/contract.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#endif
#include <fmt/core.h>
#include <fmt/format.h>
ASAP_DIAGNOSTIC_POP

#include <numeric>
#include <utility>

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
  return fmt::format("while parsing command '{}', ", command->PathAsString());
}

} // namespace

asap::clap::parser::detail::UnrecognizedCommand::~UnrecognizedCommand() =
    default;

asap::clap::parser::detail::UnrecognizedCommand::UnrecognizedCommand(
    std::vector<std::string> path_segments)
    : UnrecognizedCommand(std::move(path_segments), nullptr) {
}

asap::clap::parser::detail::UnrecognizedCommand::UnrecognizedCommand(
    std::vector<std::string> path_segments, const char *message) {
  auto description = fmt::format(
      "Unrecognized command with path '{}'", fmt::join(path_segments, " "));
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::MissingCommand::~MissingCommand() = default;

asap::clap::parser::detail::MissingCommand::MissingCommand(
    const ParserContextPtr &context)
    : MissingCommand(context, nullptr) {
}

asap::clap::parser::detail::MissingCommand::MissingCommand(
    const ParserContextPtr &context, const char *message) {
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
  StateMachineError::What(description);
}

asap::clap::parser::detail::UnrecognizedOption::~UnrecognizedOption() = default;

asap::clap::parser::detail::UnrecognizedOption::UnrecognizedOption(
    const ParserContextPtr &context, const std::string &token)
    : UnrecognizedOption(context, token, nullptr) {
}

asap::clap::parser::detail::UnrecognizedOption::UnrecognizedOption(
    const ParserContextPtr &context, const std::string &token,
    const char *message) {

  auto option_name = (token.length() == 1) ? "-" + token : "--" + token;
  auto description = fmt::format("{}'{}' is not a recognized option",
      CommandDiagnostic(context->active_command), option_name);
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::IllegalMultipleOccurrence::
    ~IllegalMultipleOccurrence() = default;

asap::clap::parser::detail::IllegalMultipleOccurrence::
    IllegalMultipleOccurrence(const ParserContextPtr &context)
    : IllegalMultipleOccurrence(context, nullptr) {
}

asap::clap::parser::detail::IllegalMultipleOccurrence::
    IllegalMultipleOccurrence(
        const ParserContextPtr &context, const char *message) {
  ASAP_EXPECT(context->active_option);
  ASAP_EXPECT(context->ovm_.OccurrencesOf(context->active_option->Key()) > 0);

  const auto &option_name = context->active_option->Key();
  auto description =
      fmt::format("{}new occurrence for option '{}' "
                  "as '{}' is illegal; it can only be used one time and it "
                  "appeared before with value '{}'",
          CommandDiagnostic(context->active_command), option_name,
          context->active_option_flag,
          context->ovm_.ValuesOf(option_name).front().OriginalToken());
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::OptionSyntaxError::~OptionSyntaxError() = default;

asap::clap::parser::detail::OptionSyntaxError::OptionSyntaxError(
    const ParserContextPtr &context)
    : OptionSyntaxError(context, nullptr) {
}

asap::clap::parser::detail::OptionSyntaxError::OptionSyntaxError(
    const ParserContextPtr &context, const char *message) {
  auto description = fmt::format("{}option '{}' is using an invalid syntax",
      CommandDiagnostic(context->active_command),
      context->active_option->Key());
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::MissingValueForOption::~MissingValueForOption() =
    default;

asap::clap::parser::detail::MissingValueForOption::MissingValueForOption(
    const ParserContextPtr &context)
    : MissingValueForOption(context, nullptr) {
}

asap::clap::parser::detail::MissingValueForOption::MissingValueForOption(
    const ParserContextPtr &context, const char *message) {
  auto description =
      fmt::format("{}option '{}' seen as '{}' "
                  "has no value on the command line and no implicit one",
          CommandDiagnostic(context->active_command),
          context->active_option->Key(), context->active_option_flag);
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::InvalidValueForOption::~InvalidValueForOption() =
    default;

asap::clap::parser::detail::InvalidValueForOption::InvalidValueForOption(
    const ParserContextPtr &context, const std::string &token)
    : InvalidValueForOption(context, token, nullptr) {
}

asap::clap::parser::detail::InvalidValueForOption::InvalidValueForOption(
    const ParserContextPtr &context, const std::string &token,
    const char *message) {

  auto description = fmt::format(
      "{}option '{}' seen as '{}',"
      " got value token '{}' which failed to parse to type {},"
      " and the option has no implicit value",
      CommandDiagnostic(context->active_command), context->active_option->Key(),
      context->active_option_flag, token, "<TODO: TYPE NAME>");
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::MissingRequiredOption::~MissingRequiredOption() =
    default;

asap::clap::parser::detail::MissingRequiredOption::MissingRequiredOption(
    const CommandPtr &command, const OptionPtr &option)
    : MissingRequiredOption(command, option, nullptr) {
}

asap::clap::parser::detail::MissingRequiredOption::MissingRequiredOption(
    const CommandPtr &command, const OptionPtr &option, const char *message) {
  auto description =
      fmt::format("{}no option '{}' was specified. "
                  "It is required and does not have a default value",
          CommandDiagnostic(command), option->Key());
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}

asap::clap::parser::detail::UnexpectedPositionals::~UnexpectedPositionals() =
    default;

asap::clap::parser::detail::UnexpectedPositionals::UnexpectedPositionals(
    const ParserContextPtr &context)
    : UnexpectedPositionals(context, nullptr) {
}

asap::clap::parser::detail::UnexpectedPositionals::UnexpectedPositionals(
    const ParserContextPtr &context, const char *message) {
  auto description = fmt::format("{}argument{} '{}' "
                                 "{} not expected by any option",
      CommandDiagnostic(context->active_command),
      context->positional_tokens.size() > 1 ? "s" : "",
      fmt::join(context->positional_tokens, ", "),
      context->positional_tokens.size() > 1 ? "are" : "is");
  AppendOptionalMessage(description, message);
  StateMachineError::What(description);
}
