//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief String manipulation helper functions, not available in the standard
 * library.
 */

#pragma once

#include <algorithm>
#include <locale>
#include <string>

namespace asap::clap::detail {

/// Return a lower case version of a string
inline auto ToLower(std::string str) -> std::string {
  std::transform(std::begin(str), std::end(str), std::begin(str),
      [](const std::string::value_type &element) {
        return std::tolower(element, std::locale());
      });
  return str;
}

} // namespace asap::clap::detail
