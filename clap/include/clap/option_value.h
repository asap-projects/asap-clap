//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief OptionValue class interface.
 */

#pragma once

#include <any>
#include <iterator>
#include <string>
#include <vector>

/// Namespace for command line parsing related APIs.
namespace asap::clap {

/*!
 * \brief Represents values for a command line option.
 *
 * This class encapsulates information about the values, how they were obtained,
 * and allows type-safe access to it.
 */
class OptionValue {
public:
  /*!
   * \brief Creates a new OptionValue.
   *
   * \param value the value that will be stored.
   * \param original_token the token from which this option value was parsed.
   * \param defaulted when \b true, indicates that the provided values have not
   * been explicitly specified but come from a default value.
   */
  OptionValue(std::any value, std::string original_token, bool defaulted)
      : value_{std::move(value)},
        original_token_(std::move(original_token)), defaulted_{defaulted} {
  }

  OptionValue(const OptionValue &) = default;
  OptionValue(OptionValue &&) = default;

  auto operator=(const OptionValue &) -> OptionValue & = delete;
  auto operator=(OptionValue &&) -> OptionValue & = delete;

  ~OptionValue() = default;

  /*!
   * \brief If the stored value has type T, returns that value; otherwise
   throws
   * std::bad_any_cast.
   */
  template <typename T> [[nodiscard]] auto GetAs() const -> const T & {
    return std::any_cast<const T &>(value_);
  }

  /*!
   * \brief If the stored value has type T, returns that value; otherwise
   throws
   * std::bad_any_cast.
   */
  template <typename T> [[nodiscard]] auto GetAs() -> T & {
    return std::any_cast<T &>(value_);
  }

  /*!
   * \brief Return true if no value is stored.
   */
  [[nodiscard]] auto IsEmpty() const -> bool {
    return !value_.has_value();
  }

  /*!
   * \brief Return true if the stored values have not been explicitly specified
   * on the command line but come from a default value.
   */
  [[nodiscard]] auto IsDefaulted() const -> bool {
    return defaulted_;
  }

  /*!
   * \brief Returns the original token from which this option value was parsed.
   */
  [[nodiscard]] auto OriginalToken() const -> const std::string & {
    return original_token_;
  }

  /*!
   * \brief Return the stored values.
   */
  [[nodiscard]] auto Value() const -> const std::any & {
    return value_;
  }

  /*!
   * \brief Return the stored values.
   */
  [[nodiscard]] auto Value() -> std::any & {
    return value_;
  }

private:
  std::any value_;
  std::string original_token_;
  bool defaulted_{false};
};

} // namespace asap::clap
