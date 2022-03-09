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

#include <clap/asap_clap_api.h>
#include <clap/option.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/// Namespace for command line parsing related APIs.
namespace asap::clap {

/*!
 * \brief A command.
 */
class ASAP_CLAP_API Command {
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
   * composed of one or more
   * **non-null** string segments. All the path segments of a command must be
   * matched in the order they are specified for the command to be selected as a
   * candidate.
   *
   * \tparam Segment we use a variadic template parameter pack for the path
   * segments, but we want all the segments to simply be `const char *` strings.
   * This is achieved by forcing the first path segment type to `const char *`.
   *
   * \param first_segment first segment of that command path; here only to force
   * the parameter pack to only accept `const char *`.
   *
   * \param other_segments zero or more path segments; must all be of type
   * `const char *`.
   *
   * \throws std::domain_error when multiple path segments are provided and one
   * of them is `""` (empty string). The default command can only have one
   * segment that must be `""`.
   *
   * **Example**
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

  Command(const Command &other) = delete;

  Command(Command &&other) noexcept = default;

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
  [[nodiscard]] auto PathAsString() const -> std::string;

  [[nodiscard]] auto About() const -> const std::string & {
    return about_;
  }
  void About(std::string about) {
    about_ = std::move(about);
  }

  auto WithOptions(std::shared_ptr<Options> options, bool hidden = false)
      -> Command & {
    for (const auto &option : options->options_) {
      options_.push_back(option);
      options_in_groups_.push_back(true);
    }
    groups_.emplace_back(std::move(options), hidden);
    return *this;
  }

  auto WithOption(OptionBuilder &builder) -> Command & {
    options_.emplace_back(builder.Build());
    options_in_groups_.push_back(false);
    return *this;
  }

  template <typename... Args,
      std::enable_if_t<std::conjunction_v<std::is_same<std::decay_t<Args>,
                           Positional<OptionBuilder>>...>,
          std::nullptr_t> = nullptr>
  auto WithPositionals(Args &&...builders) -> Command & {
    positionals_.insert(positionals_.end(), {builders.Build()...});
    return *this;
  }

  [[nodiscard]] auto FindShortOption(const std::string &name) const
      -> std::optional<std::shared_ptr<Option>> {
    auto result = std::find_if(options_.cbegin(), options_.cend(),
        [&name](const Option::Ptr &option) { return option->Short() == name; });
    if (result == options_.cend()) {
      return {};
    }
    return std::make_optional(*result);
  }

  [[nodiscard]] auto FindLongOption(const std::string &name) const
      -> std::optional<std::shared_ptr<Option>> {
    auto result = std::find_if(options_.cbegin(), options_.cend(),
        [&name](const Option::Ptr &option) { return option->Long() == name; });
    if (result == options_.cend()) {
      return {};
    }
    return std::make_optional(*result);
  }

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(
      std::ostream &out, const Command &command) -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  void Print(std::ostream &out, unsigned width = 0) const;

  void PrintOptionsSummary(std::ostream &out) const;

  [[nodiscard]] auto Options() const -> const std::vector<Option::Ptr> & {
    return options_;
  }

  [[nodiscard]] auto Positionals() const -> const std::vector<Option::Ptr> & {
    return positionals_;
  }

private:
  std::string about_;
  const std::vector<std::string> path_;
  std::vector<Option::Ptr> options_;
  std::vector<bool> options_in_groups_;
  std::vector<std::pair<Options::Ptr, bool>> groups_;
  std::vector<Option::Ptr> positionals_;
};

inline auto operator<<(std::ostream &out, const Command &command)
    -> std::ostream & {
  command.Print(out);
  return out;
}

} // namespace asap::clap
