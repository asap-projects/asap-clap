//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for Option and related types.
 */

#include <utility>

#include "clap/option.h"

namespace asap::clap {

ValueSemantics::~ValueSemantics() noexcept = default;

auto operator<<(std::ostream &out, const Option &option) -> std::ostream & {
  option.Print(out);
  return out;
}

void Option::Print(std::ostream &out, unsigned int /*width*/) const {
  out << short_name_ << " " << long_name_ << "    " << about_ << std::endl;
}

auto Option::WithName(std::string key) -> OptionBuilder {
  return OptionBuilder(std::move(key));
}

auto Option::Positional(std::string key) -> PositionalOptionBuilder {
  return PositionalOptionBuilder(std::move(key));
}

auto Option::Rest() -> PositionalOptionBuilder {
  return PositionalOptionBuilder(key_rest);
}

auto operator<<(std::ostream &out, const Options &options) -> std::ostream & {
  options.Print(out);
  return out;
}

void Options::Print(std::ostream &out, unsigned int width) const {
  if (!label_.empty()) {
    out << label_ << std::endl;
  }
  for (const auto &option : options_) {
    option->Print(out, width);
  }
}

void Options::Add(OptionBuilder &option_builder) {
  options_.push_back(option_builder.Build());
}

} // namespace asap::clap
