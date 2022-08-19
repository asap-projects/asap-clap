//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Types and macros used for clap.
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

class Cli {
public:
  auto Version(std::string version) -> Cli & {
    version_ = std::move(version);
    return *this;
  }
  [[nodiscard]] auto Version() const -> const std::string & {
    return version_;
  }

  auto About(std::string about) -> Cli & {
    about_ = std::move(about);
    return *this;
  }

  [[nodiscard]] auto About() const -> const std::string & {
    return about_;
  }

  auto ProgramName(std::string name) -> Cli & {
    program_name_ = std::move(name);
    return *this;
  }

  [[nodiscard]] auto ProgramName() const -> std::string {
    return program_name_.value_or("");
  }

  auto WithCommand(std::shared_ptr<Command> command) -> Cli & {
    commands_.push_back(std::move(command));
    return *this;
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

private:
  [[nodiscard]] auto HasHelpOption() const -> bool;

  std::string version_;
  std::string about_;
  std::optional<std::string> program_name_{};
  std::vector<std::shared_ptr<Command>> commands_;
  OptionValuesMap ovm_;
};

} // namespace asap::clap
