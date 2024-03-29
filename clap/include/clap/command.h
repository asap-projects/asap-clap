//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Declaration of the Command class which is the basic building block of
 * a command line program.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "clap/asap_clap_export.h"
#include "clap/option.h"

/// Namespace for command line parsing related APIs.
namespace asap::clap {

// Forward reference used to declare the weak pointer to the parent CLI.
class Cli;

/*!
 * \brief A command.
 */
class Command {
public:
  using Ptr = std::shared_ptr<Command>;

  /*!
   * \brief A helper to make it clearer when a command is created as the
   * *default* one (i.e. mounted at the root top-level).
   *
   * **Example**
   * \snippet command_test.cpp Default command
   */
  static constexpr const char *DEFAULT = "";

  /// Version command name.
  static constexpr const char *VERSION = "version";
  static constexpr const char *VERSION_LONG = "--version";
  static constexpr const char *VERSION_SHORT = "-v";

  /// Help command name.
  static constexpr const char *HELP = "help";
  static constexpr const char *HELP_LONG = "--help";
  static constexpr const char *HELP_SHORT = "-h";

  Command(const Command &other) = delete;
  Command(Command &&other) noexcept = delete;
  auto operator=(const Command &other) -> Command & = delete;
  auto operator=(Command &&other) noexcept -> Command & = delete;

  ~Command() = default;

  /*!
   * \brief Check if this command is the default command (i.e. mounted at the
   * root top-level).
   *
   * The default command is a command which path has one and only one segment
   * and that segment is the empty string (`""`).
   *
   * \return `true` if this is the default command; `false` otherwise.
   */
  [[nodiscard]] auto IsDefault() const -> bool {
    return path_.size() == 1 && path_.front().empty();
  }

  /*!
   * \brief Returns a vector containing the segments in this command's path in
   * the order they need to appear on the command line.
   */
  [[nodiscard]] auto Path() const -> const std::vector<std::string> & {
    return path_;
  }

  /*!
   * \brief Returns a string containing a space separated list of this command's
   * path segments in the order they need to appear on the command line.
   */
  [[nodiscard]] ASAP_CLAP_API auto PathAsString() const -> std::string;

  [[nodiscard]] auto About() const -> const std::string & {
    return about_;
  }

  [[nodiscard]] auto FindShortOption(const std::string &name) const
      -> std::optional<std::shared_ptr<Option>> {
    const auto result = std::find_if(options_.cbegin(), options_.cend(),
        [&name](const Option::Ptr &option) { return option->Short() == name; });
    if (result == options_.cend()) {
      return {};
    }
    return std::make_optional(*result);
  }

  [[nodiscard]] auto FindLongOption(const std::string &name) const
      -> std::optional<std::shared_ptr<Option>> {
    const auto result = std::find_if(options_.cbegin(), options_.cend(),
        [&name](const Option::Ptr &option) { return option->Long() == name; });
    if (result == options_.cend()) {
      return {};
    }
    return std::make_optional(*result);
  }

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend auto operator<<(std::ostream &out, const Command &command)
      -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  ASAP_CLAP_API void Print(std::ostream &out, unsigned width = 80) const;

  ASAP_CLAP_API void PrintSynopsis(std::ostream &out) const;

  ASAP_CLAP_API void PrintOptions(std::ostream &out, unsigned width) const;

  [[nodiscard]] auto CommandOptions() const
      -> const std::vector<Option::Ptr> & {
    return options_;
  }

  [[nodiscard]] auto PositionalArguments() const
      -> const std::vector<Option::Ptr> & {
    return positional_args_;
  }

  friend class CommandBuilder;
  friend class CliBuilder; // to upgrade default command with help and version

private:
  /*!
   * \brief Construct a new Command object to be mounted at the path
   * corresponding to the provided segments.
   *
   * By default a command is mounted at the top level, meaning that it starts
   * executing from the very first token in the command line arguments. This
   * corresponds to the typical command line programs that just do one specific
   * task and accept options to parametrize that task. This however does not fit
   * the scenario of command line tools that can execute multiple tasks (such as
   * `git` for example).
   *
   * To help with that, we support mounting commands at the specific path,
   * composed of one or more string segments. All the path segments of a command
   * must be matched in the order they are specified for the command to be
   * selected as a candidate.
   *
   * \tparam Segment we use a variadic template parameter pack for the path
   * segments, but we want all the segments to simply be strings. This is
   * achieved by forcing the first path segment type to string.
   *
   * \param first_segment first segment of that command path; here only to force
   * the parameter pack to only accept string.
   *
   * \param other_segments zero or more path segments; must all be of type
   * string.
   *
   * \throws std::domain_error when multiple path segments are provided and one
   * of them is `""` (empty string). The default command can only have one
   * segment that must be `""`.
   *
   * **Example**
   *
   * \snippet command_test.cpp Non-default command path
   *
   * \see DEFAULT
   */
  template <typename... Segment>
  explicit Command(std::string first_segment, Segment... other_segments)
      : path_{std::move(first_segment), std::move(other_segments)...} {
    if ((std::find(path_.begin(), path_.end(), "") != std::end(path_)) &&
        (path_.size() != 1)) {
      throw std::domain_error(
          "default command can only have one path segment (an empty string)");
    }
  }

  auto About(std::string about) -> Command & {
    about_ = std::move(about);
    return *this;
  }

  void WithOptions(std::shared_ptr<Options> options, bool hidden) {
    for (const auto &option : *options) {
      options_.push_back(option);
      options_in_groups_.push_back(true);
    }
    groups_.emplace_back(std::move(options), hidden);
  }

  void WithOption(std::shared_ptr<Option> &&option) {
    if (option->Key() == Command::HELP || option->Key() == Command::VERSION) {
      options_.emplace(options_.begin(), option);
      options_in_groups_.insert(options_in_groups_.begin(), false);
    } else {
      options_.emplace_back(option);
      options_in_groups_.push_back(false);
    }
  }

  template <typename... Args> void WithPositionalArguments(Args &&...options) {
    positional_args_.insert(
        positional_args_.end(), {std::forward<Args>(options)...});
  }

  std::string about_;
  std::vector<std::string> path_;
  std::vector<Option::Ptr> options_;
  std::vector<bool> options_in_groups_;
  std::vector<std::pair<Options::Ptr, bool>> groups_;
  std::vector<Option::Ptr> positional_args_;

  // Only updated by the CliBuilder, and only used to refer back to the parent
  // CLI to get information for better help display. Use the helper methods
  // instead of directly accessing through the pointer for better
  // maintainability.
  Cli *parent_cli_{nullptr};

  [[nodiscard]] auto ProgramName() const -> std::string;
};

inline auto operator<<(std::ostream &out, const Command &command)
    -> std::ostream & {
  command.Print(out);
  return out;
}

} // namespace asap::clap
