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

#include "clap/asap_clap_api.h"
#include "clap/option_value.h"
#include "context.h"

#include <fsm/fsm.h>

#include <optional>
#include <string>
#include <utility>

namespace asap::clap::parser::detail {

class ASAP_CLAP_API UnrecognizedCommand : public asap::fsm::StateMachineError {
public:
  UnrecognizedCommand(std::vector<std::string> path_segments);
  UnrecognizedCommand(
      std::vector<std::string> path_segments, const char *message);

  UnrecognizedCommand(const UnrecognizedCommand &) = default;
  UnrecognizedCommand(UnrecognizedCommand &&) = default;

  auto operator=(const UnrecognizedCommand &)
      -> UnrecognizedCommand & = default;
  auto operator=(UnrecognizedCommand &&) -> UnrecognizedCommand & = default;

  ~UnrecognizedCommand() override;
};

class ASAP_CLAP_API MissingCommand : public asap::fsm::StateMachineError {
public:
  explicit MissingCommand(const ParserContextPtr &context);
  MissingCommand(const ParserContextPtr &context, const char *message);

  MissingCommand(const MissingCommand &) = default;
  MissingCommand(MissingCommand &&) = default;

  auto operator=(const MissingCommand &) -> MissingCommand & = default;
  auto operator=(MissingCommand &&) -> MissingCommand & = default;

  ~MissingCommand() override;
};

class ASAP_CLAP_API UnrecognizedOption : public asap::fsm::StateMachineError {
public:
  UnrecognizedOption(const ParserContextPtr &context, const std::string &token);
  UnrecognizedOption(const ParserContextPtr &context, const std::string &token,
      const char *message);

  UnrecognizedOption(const UnrecognizedOption &) = default;
  UnrecognizedOption(UnrecognizedOption &&) = default;

  auto operator=(const UnrecognizedOption &) -> UnrecognizedOption & = default;
  auto operator=(UnrecognizedOption &&) -> UnrecognizedOption & = default;

  ~UnrecognizedOption() override;
};

class ASAP_CLAP_API MissingValueForOption
    : public asap::fsm::StateMachineError {
public:
  explicit MissingValueForOption(const ParserContextPtr &context);
  MissingValueForOption(const ParserContextPtr &context, const char *message);

  MissingValueForOption(const MissingValueForOption &) = default;
  MissingValueForOption(MissingValueForOption &&) = default;

  auto operator=(const MissingValueForOption &)
      -> MissingValueForOption & = default;
  auto operator=(MissingValueForOption &&) -> MissingValueForOption & = default;

  ~MissingValueForOption() override;
};

class ASAP_CLAP_API InvalidValueForOption
    : public asap::fsm::StateMachineError {
public:
  InvalidValueForOption(
      const ParserContextPtr &context, const std::string &token);
  InvalidValueForOption(const ParserContextPtr &context,
      const std::string &token, const char *message);

  InvalidValueForOption(const InvalidValueForOption &) = default;
  InvalidValueForOption(InvalidValueForOption &&) = default;

  auto operator=(const InvalidValueForOption &)
      -> InvalidValueForOption & = default;
  auto operator=(InvalidValueForOption &&) -> InvalidValueForOption & = default;

  ~InvalidValueForOption() override;
};

class ASAP_CLAP_API IllegalMultipleOccurrence
    : public asap::fsm::StateMachineError {
public:
  explicit IllegalMultipleOccurrence(const ParserContextPtr &context);
  IllegalMultipleOccurrence(
      const ParserContextPtr &context, const char *message);

  IllegalMultipleOccurrence(const IllegalMultipleOccurrence &) = default;
  IllegalMultipleOccurrence(IllegalMultipleOccurrence &&) = default;

  auto operator=(const IllegalMultipleOccurrence &)
      -> IllegalMultipleOccurrence & = default;
  auto operator=(IllegalMultipleOccurrence &&)
      -> IllegalMultipleOccurrence & = default;

  ~IllegalMultipleOccurrence() override;
};

class ASAP_CLAP_API OptionSyntaxError : public asap::fsm::StateMachineError {
public:
  explicit OptionSyntaxError(const ParserContextPtr &context);
  OptionSyntaxError(const ParserContextPtr &context, const char *message);

  OptionSyntaxError(const OptionSyntaxError &) = default;
  OptionSyntaxError(OptionSyntaxError &&) = default;

  auto operator=(const OptionSyntaxError &) -> OptionSyntaxError & = default;
  auto operator=(OptionSyntaxError &&) -> OptionSyntaxError & = default;

  ~OptionSyntaxError() override;
};

class ASAP_CLAP_API MissingRequiredOption
    : public asap::fsm::StateMachineError {
public:
  MissingRequiredOption(const CommandPtr &command, const OptionPtr &option);
  MissingRequiredOption(
      const CommandPtr &command, const OptionPtr &option, const char *message);

  MissingRequiredOption(const MissingRequiredOption &) = default;
  MissingRequiredOption(MissingRequiredOption &&) = default;

  auto operator=(const MissingRequiredOption &)
      -> MissingRequiredOption & = default;
  auto operator=(MissingRequiredOption &&) -> MissingRequiredOption & = default;

  ~MissingRequiredOption() override;
};

class ASAP_CLAP_API UnexpectedPositionals
    : public asap::fsm::StateMachineError {
public:
  UnexpectedPositionals(const ParserContextPtr &context);
  UnexpectedPositionals(const ParserContextPtr &context, const char *message);

  UnexpectedPositionals(const UnexpectedPositionals &) = default;
  UnexpectedPositionals(UnexpectedPositionals &&) = default;

  auto operator=(const UnexpectedPositionals &)
      -> UnexpectedPositionals & = default;
  auto operator=(UnexpectedPositionals &&) -> UnexpectedPositionals & = default;

  ~UnexpectedPositionals() override;
};

} // namespace asap::clap::parser::detail
