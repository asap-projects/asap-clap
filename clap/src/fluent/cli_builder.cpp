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

#include "clap/fluent/option_builder.h"
#include "clap/fluent/option_value_builder.h"

#include <contract/contract.h>

#include <clap/fluent/cli_builder.h>
#include <clap/fluent/command_builder.h>

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
  if (command->IsDefault()) {
    default_command_ = command;
  }
  cli_->WithCommand(std::move(command));
  return *this;
}

auto asap::clap::CliBuilder::WithVersionCommand() -> Self & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->EnableVersionCommand();
  return *this;
}

auto asap::clap::CliBuilder::WithHelpCommand() -> Self & {
  ASAP_ASSERT(cli_ && "builder used after Build() was called");
  cli_->EnableHelpCommand();
  return *this;
}

/// Explicitly get the encapsulated `Cli` instance.
auto asap::clap::CliBuilder::Build() -> std::unique_ptr<Cli> {
  if (cli_->HasHelpCommand() || cli_->HasVersionCommand()) {
    if (!default_command_) {
      CommandBuilder command_builder(Command::DEFAULT);
      WithCommand(command_builder);
    }
    if (cli_->HasVersionCommand()) {
      default_command_->WithOption(Option::WithName("version")
                                       .About("display version information")
                                       .Short("v")
                                       .Long("version")
                                       .WithValue<bool>()
                                       .Build());
    }
    if (cli_->HasHelpCommand()) {
      default_command_->WithOption(
          Option::WithName("help")
              .About("display detailed help information")
              .Short("h")
              .Long("help")
              .WithValue<bool>()
              .Build());
    }
  }
  return std::move(cli_);
}
