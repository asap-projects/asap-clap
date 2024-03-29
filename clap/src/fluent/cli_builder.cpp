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

#include "clap/fluent/cli_builder.h"
#include "clap/command.h"
#include "clap/fluent/command_builder.h"
#include "clap/fluent/option_builder.h"
#include "clap/fluent/option_value_builder.h"

#include <memory>

#include <common/compilers.h>
#include <contract/contract.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#endif
#if defined(ASAP_GNUC_VERSION)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include <fmt/core.h>
ASAP_DIAGNOSTIC_POP

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

void asap::clap::CliBuilder::AddHelpOptionToCommand(Command &command) {
  command.WithOption(
      Option::WithKey("help")
          .About(
              fmt::format("Display detailed help information.\nNote "
                          "that `{} --help` is identical to `{} help` because "
                          "the former is internally converted into the latter.",
                  cli_->ProgramName(), cli_->ProgramName(), cli_->ProgramName(),
                  cli_->ProgramName()))
          .Short("h")
          .Long("help")
          .WithValue<bool>()
          .Build());
}

void asap::clap::CliBuilder::AddVersionOptionToCommand(Command &command) {
  command.WithOption(
      Option::WithKey("version")
          .About(
              fmt::format("Display version information.\nNote that `{} "
                          "--version` is identical to `{} version` because "
                          "the former is internally converted into the latter.",
                  cli_->ProgramName(), cli_->ProgramName()))
          .Short("v")
          .Long("version")
          .WithValue<bool>()
          .Build());
}

auto asap::clap::CliBuilder::Build() -> std::unique_ptr<Cli> {
  // Handle additional setup needed when the default `version` or `help`
  // commands are enabled.

  if (cli_->HasHelpCommand() || cli_->HasVersionCommand()) {
    auto has_default_command = false;

    for (auto &command : cli_->commands_) {
      if (cli_->HasHelpCommand()) {
        AddHelpOptionToCommand(*command);
      }
      if (command->IsDefault()) {
        has_default_command = true;
        if (cli_->HasVersionCommand()) {
          AddVersionOptionToCommand(*command);
        }
      }
    }

    // If the CLI if did not have a default command, create one and set it up.
    if (!has_default_command) {
      const std::shared_ptr<Command> command =
          CommandBuilder(cli_->ProgramName(), Command::DEFAULT);
      if (cli_->HasHelpCommand()) {
        AddHelpOptionToCommand(*command);
      }
      if (cli_->HasVersionCommand()) {
        AddVersionOptionToCommand(*command);
      }
      WithCommand(command);
    }
  }

  // Update all CLI commands to have a weak reference to the parent CLI
  for (auto &command : cli_->commands_) {
    command->parent_cli_ = cli_.get();
  }

  return std::move(cli_);
}
