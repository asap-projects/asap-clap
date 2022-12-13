//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief The main entry point for the asap-clap library.
 */

#pragma once

#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "clap/asap_clap_export.h"
#include "clap/command.h"
#include "clap/option_values_map.h"

/// Namespace for command line parsing related APIs.
namespace asap::clap {

/*!
 * \brief An exception thrown when a command line arguments parsing error
 * occurs.
 *
 * Note that this exception indicates an unrecoverable error and nothing much is
 * expected from the caller to handle such error except failing.
 */
class CmdLineArgumentsError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
  CmdLineArgumentsError(const CmdLineArgumentsError &) = default;
  CmdLineArgumentsError(CmdLineArgumentsError &&) noexcept = default;
  auto operator=(const CmdLineArgumentsError &)
      -> CmdLineArgumentsError & = default;
  auto operator=(CmdLineArgumentsError &&) noexcept
      -> CmdLineArgumentsError & = default;
  ~CmdLineArgumentsError() override;
};

class CliBuilder;

/*!
 * \brief The main entry point of the command line arguments parsing API.
 *
 * To parse command line arguments, use a CliBuilder to create a `Cli`,
 * configure its different options and add commands to it. Once built, you can
 * call `Parse()` with the program command line arguments.
 */
class Cli {
public:
  /// The program version string.
  [[nodiscard]] auto Version() const -> const std::string & {
    return version_;
  }

  /// A descriptive message about this command line program.
  [[nodiscard]] auto About() const -> const std::string & {
    return about_;
  }

  /*!
   * \brief The program name.
   *
   * This is either set explicitly using the builder's
   * `CLiBuilder::ProgramName()` method or deduced from the command line
   * arguments array. In the latter case, the value is only available after a
   * call to `Parse()`.
   */
  [[nodiscard]] auto ProgramName() const -> std::string {
    return program_name_.value_or("");
  }

  ASAP_CLAP_API auto Parse(int argc, const char **argv)
      -> const OptionValuesMap &;

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(std::ostream &out, const Cli &cli)
      -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  ASAP_CLAP_API void Print(std::ostream &out, unsigned width = 0) const;

  // Cli instances are created and configured only via the associated
  // CliBuilder.
  friend class CliBuilder;

private:
  Cli() = default;

  void Version(std::string version) {
    version_ = std::move(version);
  }

  void About(std::string about) {
    about_ = std::move(about);
  }

  void ProgramName(std::string name) {
    program_name_ = std::move(name);
  }

  void WithCommand(std::shared_ptr<Command> command) {
    commands_.push_back(std::move(command));
  }

  [[nodiscard]] auto HasHelpOption() const -> bool;

  std::string version_;
  std::string about_;
  std::optional<std::string> program_name_{};
  std::vector<std::shared_ptr<Command>> commands_;
  OptionValuesMap ovm_;
};

} // namespace asap::clap
