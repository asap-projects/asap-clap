//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for the cli builder fluent API.
 */

#include <contract/contract.h>

#include "clap/fluent/command_builder.h"

auto asap::clap::CommandBuilder::About(std::string about) -> Self & {
  ASAP_ASSERT(command_ && "builder used after Build() was called");
  command_->About(std::move(about));
  return *this;
}

auto asap::clap::CommandBuilder::WithOption(std::shared_ptr<Option> option)
    -> Self & {
  ASAP_ASSERT(command_ && "builder used after Build() was called");
  command_->WithOption(std::move(option));
  return *this;
}

auto asap::clap::CommandBuilder::WithOptions(
    std::shared_ptr<Options> options, bool hidden) -> Self & {
  ASAP_ASSERT(command_ && "builder used after Build() was called");
  command_->WithOptions(std::move(options), hidden);
  return *this;
}
