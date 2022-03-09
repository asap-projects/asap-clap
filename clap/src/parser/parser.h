//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Command line arguments parser.
 */

#pragma once

#include "tokenizer.h"

#include "./context.h"
#include "clap/asap_clap_api.h"
#include "clap/command.h"

#include <map>
#include <optional>
#include <utility>

namespace asap::clap::parser {

class ASAP_CLAP_API CmdLineParser {
public:
  using CommandsList = const std::vector<Command::Ptr>;
  explicit CmdLineParser(const CommandLineContext &context,
      const Tokenizer &tokenizer, CommandsList &commands)
      : tokenizer_(tokenizer), context_{detail::ParserContext::New(
                                   context, commands)} {
  }

  auto Parse() -> bool;

private:
  const Tokenizer &tokenizer_;
  const detail::ParserContextPtr context_;
};

} // namespace asap::clap::parser
