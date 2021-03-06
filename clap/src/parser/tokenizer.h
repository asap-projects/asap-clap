//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Tokenizer for command line arguments parsing.
 */

#pragma once

#include <clap/asap_clap_api.h>
#include <common/compilers.h>

// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
ASAP_DIAGNOSTIC_PUSH
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
#include <fmt/core.h>
ASAP_DIAGNOSTIC_POP

#include <magic_enum.hpp>

#include <deque>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace asap::clap::parser {

enum class TokenType {
  ShortOption,
  LongOption,
  LoneDash,
  DashDash,
  Value,
  EqualSign,
  EndOfInput
};

ASAP_CLAP_API auto operator<<(std::ostream &out, const TokenType &token_type)
    -> std::ostream &;

using Token = std::pair<TokenType, std::string>;

/*!
 * \brief Transform a list of command line arguments into a stream of typed
 * tokens for later processing by the command line parser.
 *
 * **Example**
 *
 * \snippet tokenizer_test.cpp Tokenizer example
 */
class Tokenizer {
public:
  /*!
   * \brief Make a tokenizer with the given command line arguments.
   *
   * When calling this from a main function with argc/argv, remove the program
   * name (argv[0]) from the command line arguments before passing the remaining
   * arguments to the tokenizer.
   */
  explicit ASAP_CLAP_API Tokenizer(std::vector<std::string> args)
      : args_{std::move(args)}, cursor_{args_.begin()} {
  }

  ASAP_CLAP_API auto NextToken() const -> Token {
    if (!tokens_.empty()) {
      auto token = tokens_.front();
      tokens_.pop_front();
      return token;
    }
    if (cursor_ != args_.end()) {
      auto arg = *cursor_++;
      Tokenize(arg);
      if (!tokens_.empty()) {
        auto token = tokens_.front();
        tokens_.pop_front();
        return token;
      }
    }
    return Token{TokenType::EndOfInput, ""};
  }

  ASAP_CLAP_API auto HasMoreTokens() -> bool {
    return !tokens_.empty() || cursor_ != args_.end();
  }

private:
  ASAP_CLAP_API void Tokenize(const std::string &arg) const;

  const std::vector<std::string> args_;
  mutable std::vector<std::string>::const_iterator cursor_;
  mutable std::deque<Token> tokens_;
};

} // namespace asap::clap::parser

#if !defined(DOXYGEN_DOCUMENTATION_BUILD)
template <> struct fmt::formatter<asap::clap::parser::TokenType> {
  template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(
      const asap::clap::parser::TokenType &token_type, FormatContext &ctx) {
    return fmt::format_to(ctx.out(), "{}", magic_enum::enum_name(token_type));
  }
};
#endif // DOXYGEN_DOCUMENTATION_BUILD
