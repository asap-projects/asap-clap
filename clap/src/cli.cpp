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
#include <gsl/gsl>
#include <textwrap/textwrap.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#endif
#include <fmt/core.h>
ASAP_DIAGNOSTIC_POP

#include "clap/detail/args.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"

using asap::clap::detail::Arguments;

namespace asap::clap {

CmdLineArgumentsError::~CmdLineArgumentsError() = default;

auto Cli::Parse(int argc, const char **argv) -> const OptionValuesMap & {
  const Arguments cla{argc, argv};

  if (!program_name_) {
    program_name_ = cla.ProgramName();
  }

  const parser::Tokenizer tokenizer{cla.Args()};
  const CommandLineContext context(ProgramName(), ovm_);
  parser::CmdLineParser parser(context, tokenizer, commands_);
  if (parser.Parse()) {
    return ovm_;
  }
  if (HasHelpOption()) {
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
auto Cli::HasHelpOption() const -> bool {
  for (const auto &command : commands_) {
    if (command->IsDefault()) {
      return command->FindLongOption("help").has_value();
    }
  }
  return false;
}

} // namespace asap::clap
