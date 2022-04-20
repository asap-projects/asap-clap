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

#include <clap/asap_clap_api.h>
#include <clap/command.h>
#include <clap/option_values_map.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/// Namespace for command line parsing related APIs.
namespace asap::clap {

class CmdLineArgumentsError {};

class ArgumentsImpl;

class Arguments {
public:
  /*!
   * \brief Constructor to automatically covert from {argc, argv} to safer
   * types.
   *
   * \param [in] argc Non-negative value representing the number of arguments
   * passed to the program from the environment in which the program is run.
   *
   * \param [in] argv Pointer to the first element of an array of `argc + 1`
   * pointers, of which the last one is null and the previous ones, if any,
   * point to null-terminated multibyte strings that represent the arguments
   * passed to the program from the execution environment. If `argv[0]` is not a
   * null pointer (or, equivalently, if `argc > 0`), it points to a string that
   * represents the name used to invoke the program, or to an empty string.
   */
  ASAP_CLAP_API Arguments(int argc, const char **argv);

  /*!
   * \brief The program name, originally provided as the first element of the
   * `argv` array.
   */
  [[nodiscard]] ASAP_CLAP_API auto ProgramName() const -> const std::string &;

  /*!
   * \brief The program command line arguments, excluding the program name.
   *
   * \see ProgramName
   */
  [[nodiscard]] ASAP_CLAP_API auto Args() const
      -> const std::vector<std::string> &;

private:
  // Stores the implementation and the implementation's deleter as well.
  // Deleter is a pointer to a function with signature
  // `void func(ArgumentsImpl *)`.
  // https://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
  std::unique_ptr<ArgumentsImpl, void (*)(ArgumentsImpl *)> impl_;
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
