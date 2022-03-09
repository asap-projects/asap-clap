//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Describes the usage interface for commands.
 */

#pragma once

#include "clap/asap_clap_api.h"
#include <mixin/mixin.h>

#include <string>
#include <utility>

namespace asap::clap {

template <typename Base> struct WithUsageDetails : Base {
  template <typename... Args>
  explicit WithUsageDetails(std::string details, Args &&...rest)
      : Base(std::forward<Args>(rest)...), usage_details{std::move(details)} {
  }
  const std::string usage_details{};
};

template <typename Base> struct WithUsageExamples : Base {
  template <typename... Args>
  explicit WithUsageExamples(std::string examples, Args &&...rest)
      : Base(std::forward<Args>(rest)...), usage_examples{std::move(examples)} {
  }
  const std::string usage_examples{};
};

} // namespace asap::clap
