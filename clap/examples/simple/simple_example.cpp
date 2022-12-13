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
#include "clap/fluent/command_builder.h"
#include "clap/fluent/dsl.h"

using asap::clap::Cli;
using asap::clap::CliBuilder;
using asap::clap::Command;
using asap::clap::CommandBuilder;
using asap::clap::Option;

auto main(int argc, const char **argv) -> int {
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
        Option::WithName("quiet")
            .About("don't print anything to the standard output")
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
        Option::WithName("lines")
            .About("print the first NUM lines instead of the first 10; with "
                   "the leading '-', print all but the last  NUM lines of "
                   "each file")
            .Short("n")
            .Long("lines")
            .WithValue<int>()
            .DefaultValue(default_num_lines)
            .Build());
    //! [ComplexOption example]

    const std::unique_ptr<Cli> cli =
        CliBuilder()
            .ProgramName("simple-cli")
            .Version("1.0.0")
            .About("A simple command line example.")
            .WithCommand(command_builder);

    const auto &ovm = cli->Parse(argc, argv);
    if (!quiet) {
      std::cout << "-- Simple command line invoked, value of `lines` is: "
                << ovm.ValuesOf("lines").at(0).GetAs<int>() << std::endl;
    }
  } catch (...) {
    return -1;
  }
}
