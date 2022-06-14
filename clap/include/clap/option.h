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
#include <clap/detail/parse_value.h>
#include <clap/value_semantics.h>

#include <contract/contract.h>

#include <any>
#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace asap::clap {

class OptionBuilder;
template <typename Builder,
    std::enable_if_t<std::is_same_v<Builder, OptionBuilder>, std::nullptr_t> =
        nullptr>
class Positional;
using PositionalOptionBuilder = Positional<OptionBuilder>;

class Option {
public:
  constexpr static const char *key_rest = "_REST_";

  using Ptr = std::shared_ptr<Option>;

  [[nodiscard]] ASAP_CLAP_API auto Short() const -> const std::string & {
    return short_name_;
  }
  ASAP_CLAP_API void Short(std::string short_name) {
    short_name_ = std::move(short_name);
  }

  [[nodiscard]] ASAP_CLAP_API auto Long() const -> const std::string & {
    return long_name_;
  }
  ASAP_CLAP_API void Long(std::string long_name) {
    long_name_ = std::move(long_name);
  }

  [[nodiscard]] ASAP_CLAP_API auto About() const -> const std::string & {
    return about_;
  }
  ASAP_CLAP_API void About(std::string about) {
    about_ = std::move(about);
  }

  [[nodiscard]] ASAP_CLAP_API auto Key() const -> const std::string & {
    return key_;
  }

  [[nodiscard]] ASAP_CLAP_API auto IsPositional() const -> bool {
    return long_name_.empty() && short_name_.empty();
  }

  [[nodiscard]] ASAP_CLAP_API auto IsPositionalRest() const -> bool {
    return IsPositional() && key_ == key_rest;
  }

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(std::ostream &out, const Option &option)
      -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  ASAP_CLAP_API void Print(std::ostream &out, unsigned width = 0) const;

  static ASAP_CLAP_API auto WithName(std::string key) -> OptionBuilder;
  static ASAP_CLAP_API auto Positional(std::string key)
      -> PositionalOptionBuilder;
  static ASAP_CLAP_API auto Rest() -> PositionalOptionBuilder;

  /// Semantic of option's value
  [[nodiscard]] ASAP_CLAP_API auto value_semantic() const
      -> std::shared_ptr<const ValueSemantics> {
    return value_semantic_;
  }

  friend class OptionBuilder;
  template <typename> friend class OptionValueBuilder;

private:
  std::string key_;
  std::string short_name_;
  std::string long_name_;
  std::string about_;
  // shared_ptr is needed to simplify memory management in
  // copy ctor and destructor.
  std::shared_ptr<const ValueSemantics> value_semantic_;

  explicit Option(std::string key) : key_(std::move(key)) {
  }

  template <typename T>
  ASAP_CLAP_TEMPLATE_API Option(
      std::string key, std::unique_ptr<ValueSemantics> value_semantic)
      : key_(std::move(key)), value_semantic_(std::move(value_semantic)) {
  }
};

struct Options {
  using Ptr = std::shared_ptr<Options>;
  using OptionsCollectionType = std::vector<std::shared_ptr<Option>>;

  ASAP_CLAP_API explicit Options(std::string label) : label_{std::move(label)} {
  }

  /** Adds new variable description. Throws duplicate_variable_error if
      either short or long name matches that of already present one.
  */
  ASAP_CLAP_API void Add(std::shared_ptr<Option> option);

  ASAP_CLAP_API auto begin() -> OptionsCollectionType::iterator {
    return options_.begin();
  }
  ASAP_CLAP_API auto end() -> OptionsCollectionType::iterator {
    return options_.end();
  }

  [[nodiscard]] ASAP_CLAP_API auto cbegin() const noexcept
      -> OptionsCollectionType::const_iterator {
    return options_.cbegin();
  }
  [[nodiscard]] ASAP_CLAP_API auto cend() const noexcept
      -> OptionsCollectionType::const_iterator {
    return options_.cend();
  }

  /** Produces a human readable output of 'desc', listing options,
      their descriptions and allowed parameters. Other options_description
      instances previously passed to add will be output separately. */
  friend ASAP_CLAP_API auto operator<<(
      std::ostream &out, const Options &options) -> std::ostream &;

  /** Outputs 'desc' to the specified stream, calling 'f' to output each
      option_description element. */
  ASAP_CLAP_API void Print(std::ostream &out, unsigned width = 0) const;

private:
  const std::string label_;
  OptionsCollectionType options_;
};

} // namespace asap::clap
