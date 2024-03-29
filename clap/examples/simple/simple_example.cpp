//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief A simple command line example to demonstrate the commonly used
 * capabilities of asap::clap library.
 */

#include <iostream>

#include "clap/cli.h"
#include "clap/command_line_context.h"
#include "clap/fluent/command_builder.h"
#include "clap/fluent/dsl.h"

using asap::clap::Cli;
using asap::clap::CliBuilder;
using asap::clap::Command;
using asap::clap::CommandBuilder;
using asap::clap::Option;

auto main(int argc, const char **argv) -> int {
  std::unique_ptr<Cli> cli;
  try {
    bool quiet{false};

    // Describe the `default` command for this program.
    // We could also use a specific command by providing a specific name
    // when creating the command.

    CommandBuilder command_builder(Command::DEFAULT);
    //! [SimpleOptionFlag example]
    command_builder.WithOption(
        // Define a boolean flag option to configure `quiet` mode for
        // the program
        Option::WithKey("quiet")
            .About("Don't print anything to the standard output.")
            .Short("q")
            .Long("quiet")
            .WithValue<bool>()
            .StoreTo(&quiet)
            .Build());

    //! [SimpleOptionFlag example]

    //! [ComplexOption example]
    constexpr const int default_num_lines = 10;
    command_builder.WithOption(
        // Define an option to control a more sophisticated program
        // configuration parameter
        Option::WithKey("lines")
            .About("Print the first <num> lines instead of the first 10 (by "
                   "default); with the leading '-', print all but the last "
                   "<num> lines of each file.")
            .Short("n")
            .Long("lines")
            .WithValue<int>()
            .DefaultValue(default_num_lines)
            .UserFriendlyName("num")
            .Build());
    //! [ComplexOption example]

    cli = CliBuilder()
              .ProgramName("simple-cli")
              .Version("1.0.0")
              .About("This is a simple command line example to demonstrate the "
                     "commonly used features of `asap-clap`. It uses the "
                     "standard `version` and `help` commands and only "
                     "implements a default command with several options.")
              .WithVersionCommand()
              .WithHelpCommand()
              .WithCommand(command_builder);

    const auto context = cli->Parse(argc, argv);
    const auto command_path = context.active_command->PathAsString();

    const auto &ovm = context.ovm;

    if (command_path == Command::VERSION || command_path == Command::HELP ||
        ovm.HasOption(Command::HELP)) {
      return 0;
    }

    if (!quiet) {
      std::cout << "-- Simple command line invoked, value of `lines` is: "
                << ovm.ValuesOf("lines").at(0).GetAs<int>() << std::endl;
    }
  } catch (...) {
    return -1;
  }
}
