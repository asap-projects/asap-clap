//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for the clap CommandLine.
 */

#include "clap/cli.h"

#include <sstream>

#include <common/compilers.h>
#include <textwrap/textwrap.h>

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

#include "clap/detail/args.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"
#include <clap/fluent/command_builder.h>

using asap::clap::detail::Arguments;

namespace asap::clap {

CmdLineArgumentsError::~CmdLineArgumentsError() = default;

auto Cli::Parse(int argc, const char **argv) -> CommandLineContext {
  const Arguments cla{argc, argv};

  if (!program_name_) {
    program_name_ = cla.ProgramName();
  }

  auto &args = cla.Args();

  if (!args.empty()) {
    std::string &first = args[0];
    if (has_version_command_ &&
        (first == Command::VERSION_SHORT || first == Command::VERSION_LONG)) {
      first.assign(Command::VERSION);
    } else if (has_help_command_ &&
               (first == Command::HELP_SHORT || first == Command::HELP_LONG)) {
      first.assign(Command::HELP);
    }
  }

  const parser::Tokenizer tokenizer{cla.Args()};
  CommandLineContext context(ProgramName(), active_command_, ovm_);
  parser::CmdLineParser parser(context, tokenizer, commands_);
  if (parser.Parse()) {
    if (context.active_command->PathAsString() == "version") {
      context.out_ << fmt::format(
                          "{} version {}\n", program_name_.value(), version_)
                   << std::endl;
    } else if (context.active_command->PathAsString() == "help") {
      Print(context.out_);
    }

    return context;
  }
  if (HasHelpCommand()) {
    context.out_ << fmt::format("Try '{} --help' for more information.",
                        program_name_.value())
                 << std::endl;
  }
  throw CmdLineArgumentsError(
      fmt::format("command line arguments parsing failed, try '{} --help' for "
                  "more information.",
          program_name_.value()));
}
auto operator<<(std::ostream &out, const Cli &cli) -> std::ostream & {
  cli.Print(out);
  return out;
}
void Cli::Print(std::ostream &out, unsigned int width) const {
  wrap::TextWrapper wrap =
      wrap::TextWrapper::Create().Width(width).CollapseWhiteSpace().TrimLines();
  out << wrap.Fill(about_).value();
  std::ostringstream ostr;
  for (const auto &command : commands_) {
    out << "\n\n";
    command->PrintOptionsSummary(ostr);
    std::string indent = fmt::format("{}{} ", ProgramName(),
        command->IsDefault() ? "" : " " + command->PathAsString());
    std::string indent_next(indent.size(), ' ');
    wrap::TextWrapper command_wrap = wrap::TextWrapper::Create()
                                         .Width(width)
                                         .TrimLines()
                                         .IndentWith()
                                         .Initially(indent)
                                         .Then(indent_next);
    out << command_wrap.Fill(ostr.str()).value();
    if (!command->About().empty()) {
      out << "\n";
      indent_next.append("  ");
      wrap::TextWrapper command_about_wrap = wrap::TextWrapper::Create()
                                                 .Width(width)
                                                 .TrimLines()
                                                 .IndentWith()
                                                 .Initially("  ")
                                                 .Then("  ");
      out << command_about_wrap.Fill(command->About()).value();
    }
    ostr.str("");
    ostr.clear();
  }
  out << "\n\n";
}

void Cli::EnableVersionCommand() {
  CommandBuilder command_builder(Command::VERSION);
  commands_.push_back(std::move(command_builder));
  has_version_command_ = true;
}

void Cli::EnableHelpCommand() {
  CommandBuilder command_builder(Command::HELP);
  commands_.push_back(std::move(command_builder));
  has_help_command_ = true;
}

} // namespace asap::clap
