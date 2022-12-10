//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief A safer type to encapsulate the program's `argc` and `argv`.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "clap/asap_clap_export.h"

namespace asap::clap::detail {

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
   * point to null-terminated multi-byte strings that represent the arguments
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
  class ArgumentsImpl;

  // Stores the implementation and the implementation's deleter as well to work
  // around the fact that the implementation class is an incomplete type so far.
  // Deleter is a pointer to a function with signature
  // `void func(ArgumentsImpl *)`.
  // https://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
  const std::unique_ptr<ArgumentsImpl, void (*)(ArgumentsImpl *)> impl_;
};

} // namespace asap::clap::detail
