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

#include "parser/parser.h"
#include "parser/tokenizer.h"

#include <clap/cli.h>
#include <contract/contract.h>

#include <textwrap/textwrap.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#if ASAP_HAS_WARNING("-Wreserved-identifier")
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#endif
#include <fmt/core.h>
ASAP_DIAGNOSTIC_POP

#include <gsl/gsl>

namespace asap::clap {

class ArgumentsImpl {
public:
  ArgumentsImpl(int argc, const char **argv) {
    ASAP_EXPECT(argc > 0);
    ASAP_EXPECT(argv != nullptr);

    auto s_argv = gsl::span(argv, static_cast<size_t>(argc));
    // Extract the program name from the first argument (should always be there)
    // and keep the rest of the arguments for later parsing
    program_name.assign(s_argv[0]);
    if (argc > 1) {
      args.assign(++s_argv.begin(), s_argv.end());
    }

    ASAP_ENSURE(!program_name.empty());
  }

  std::string program_name;
  std::vector<std::string> args{};
};

Arguments::Arguments(int argc, const char **argv)
    : impl_(new ArgumentsImpl(argc, argv),
          // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
          [](ArgumentsImpl *impl) { delete impl; }) {
}

auto Arguments::ProgramName() const -> const std::string & {
  return impl_->program_name;
}
auto Arguments::Args() const -> const std::vector<std::string> & {
  return impl_->args;
}

auto Cli::Parse(int argc, const char **argv) -> const OptionValuesMap & {
  Arguments cla{argc, argv};

  if (!program_name_) {
    program_name_ = cla.ProgramName();
  }

  const parser::Tokenizer tokenizer{cla.Args()};
  CommandLineContext context(ProgramName(), ovm_);
  parser::CmdLineParser parser(context, tokenizer, commands_);
  if (parser.Parse()) {
    return ovm_;
  }
  if (HasHelpOption()) {
    context.out_ << fmt::format("Try '{} --help' for more information.",
                        program_name_.value())
                 << std::endl;
  }
  throw CmdLineArgumentsError();
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
