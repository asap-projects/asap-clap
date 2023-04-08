//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for the Command class.
 */

#include "clap/command.h"

#include "clap/fluent/command_builder.h"

#include <common/compilers.h>
#include <textwrap/textwrap.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(ASAP_CLANG_VERSION)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif
#if defined(ASAP_GNUC_VERSION)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include <fmt/core.h>
#include <fmt/format.h>
ASAP_DIAGNOSTIC_POP

void asap::clap::Command::PrintOptionsSummary(std::ostream &out) const {
  for (const auto &option : options_) {
    out << (option->value_semantic()->IsRequired() ? "" : "[");
    if (!option->Short().empty()) {
      out << "-" << option->Short();
      if (!option->Long().empty()) {
        out << ",";
      }
    }
    if (!option->Long().empty()) {
      out << "--" << option->Long();
    }
    out << (option->value_semantic()->IsRequired() ? " " : "] ");
  }
  for (const auto &positional : positional_args_) {
    out << " " << (positional->value_semantic()->IsRequired() ? "" : "[")
        << positional->Key()
        << (positional->value_semantic()->IsRequired() ? "" : "]");
  }

  if (IsDefault()) {
    out << "<command> [<args>]";
  } else {
    if (!positional_args_.empty()) {
      out << "[<args>]";
    }
  }
}

void asap::clap::Command::PrintOptions(
    std::ostream &out, unsigned int width) const {

  for (unsigned option_index = 0; option_index < options_.size();
       ++option_index) {
    if (options_in_groups_[option_index]) {
      continue;
    }

    if (option_index > 0) {
      out << "\n\n";
    }
    options_[option_index]->Print(out, width);
  }

  for (auto [group, hidden] : groups_) {
    if (!hidden) {
      out << "\n\n";
      group->Print(out, width);
    }
  }
}

void asap::clap::Command::Print(std::ostream &out, unsigned int width) const {
  // TODO(Abdessattar) print the command summary

  wrap::TextWrapper wrap =
      wrap::TextWrapper::Create().Width(width).CollapseWhiteSpace().TrimLines();
  out << wrap.Fill(about_).value();

  for (unsigned option_index = 0; option_index < options_.size();
       ++option_index) {
    if (options_in_groups_[option_index]) {
      continue;
    }

    options_[option_index]->Print(out, width);

    out << "\n";
  }

  for (auto [group, hidden] : groups_) {
    if (!hidden) {
      out << "\n";
      group->Print(out, width);
    }
  }
}

auto asap::clap::Command::PathAsString() const -> std::string {
  return fmt::format("{}", fmt::join(path_, " "));
}
