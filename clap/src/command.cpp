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

#include <common/compilers.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#pragma clang diagnostic ignored "-Wundefined-func-template"
#endif
#include <fmt/core.h>
#include <fmt/format.h>
ASAP_DIAGNOSTIC_POP

using asap::clap::Command;

void asap::clap::Command::PrintOptionsSummary(
    std::ostream &out) const {
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
  for (const auto &positional : positionals_) {
    out << " " << (positional->value_semantic()->IsRequired() ? "" : "[")
        << positional->Key()
        << (positional->value_semantic()->IsRequired() ? "" : "]");
  }
}

void asap::clap::Command::Print(std::ostream &out, unsigned int width) const {
  // TODO(Abdessattar) print the command summary

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
