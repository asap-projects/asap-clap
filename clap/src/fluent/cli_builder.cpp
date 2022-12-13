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

#include "clap/fluent/cli_builder.h"

auto asap::clap::CliBuilder::Version(std::string version) -> CliBuilder & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->Version(std::move(version));
  return *this;
}

auto asap::clap::CliBuilder::ProgramName(std::string name) -> CliBuilder & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->ProgramName(std::move(name));
  return *this;
}

auto asap::clap::CliBuilder::About(std::string about) -> CliBuilder & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->About(std::move(about));
  return *this;
}

auto asap::clap::CliBuilder::WithCommand(std::shared_ptr<Command> command)
    -> CliBuilder & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->WithCommand(std::move(command));
  return *this;
}
