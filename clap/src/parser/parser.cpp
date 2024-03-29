//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Command line arguments parser implementation details.
 */

#include "parser.h"

#include <contract/contract.h>
#include <fsm/fsm.h>
#include <logging/logging.h>

#include "common/compilers.h"
#include "events.h"
#include "states.h"
#include "tokenizer.h"

using asap::fsm::Continue;
using asap::fsm::ReissueEvent;
using asap::fsm::Status;
using asap::fsm::Terminate;
using asap::fsm::TerminateWithError;

using asap::clap::parser::detail::DashDashState;
using asap::clap::parser::detail::FinalState;
using asap::clap::parser::detail::IdentifyCommandState;
using asap::clap::parser::detail::InitialState;
using asap::clap::parser::detail::Machine;
using asap::clap::parser::detail::ParseLongOptionState;
using asap::clap::parser::detail::ParseOptionsState;
using asap::clap::parser::detail::ParseShortOptionState;
using asap::clap::parser::detail::TokenEvent;

/*!
 * \brief The Overload pattern allows to explicitly 'overload' lambdas and is
 * particularly useful for creating visitors, e.g. for std::variant.
 */
template <typename... Ts> // (7)
struct Overload : Ts... {
  using Ts::operator()...;
};
// Deduction guide only needed for C++17. C++20 can automatically create the
// template parameters out of the constructor arguments.
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

auto asap::clap::parser::CmdLineParser::Parse() -> bool {
  auto &logger = asap::logging::Registry::GetLogger("CmdLineParser");

  auto machine = Machine{InitialState{context_}, IdentifyCommandState{},
      ParseOptionsState{}, ParseShortOptionState{}, ParseLongOptionState{},
      DashDashState{}, FinalState{}};

  bool continue_running{true};
  bool no_errors{true};
  auto token = tokenizer_.NextToken();
  bool reissue = false;
  do {
    const auto &[token_type, token_value] = token;
    ASLOG_TO_LOGGER(
        logger, debug, "next event: {}/{}", token.first, token.second);

    Status execution_status;

    switch (token_type) {
    case TokenType::ShortOption:
      execution_status =
          machine.Handle(TokenEvent<TokenType::ShortOption>{token_value});
      break;
    case TokenType::LongOption:
      execution_status =
          machine.Handle(TokenEvent<TokenType::LongOption>{token_value});
      break;
    case TokenType::LoneDash:
      execution_status =
          machine.Handle(TokenEvent<TokenType::LoneDash>{token_value});
      break;
    case TokenType::DashDash:
      execution_status =
          machine.Handle(TokenEvent<TokenType::DashDash>{token_value});
      break;
    case TokenType::EqualSign:
      execution_status =
          machine.Handle(TokenEvent<TokenType::EqualSign>{token_value});
      break;
    case TokenType::Value:
      execution_status =
          machine.Handle(TokenEvent<TokenType::Value>{token_value});
      break;
    case TokenType::EndOfInput:
      execution_status =
          machine.Handle(TokenEvent<TokenType::EndOfInput>{token_value});
      break;
    default:
      ASAP_UNREACHABLE();
    }

    // https://en.cppreference.com/w/cpp/utility/variant/visit
    std::visit(Overload{
                   [&continue_running](const Continue & /*status*/) noexcept {
                     continue_running = true;
                   },
                   [&continue_running](const Terminate & /*status*/) noexcept {
                     continue_running = false;
                   },
                   //  [this, &continue_running, &no_errors, &logger](
                   [this, &continue_running, &no_errors, &logger](
                       const TerminateWithError &status) {
                     ASLOG_TO_LOGGER(logger, error, "{}", status.error_message);
                     context_->err_
                         << fmt::format("{}: {}", context_->program_name_,
                                status.error_message)
                         << std::endl;
                     continue_running = false;
                     no_errors = false;
                   },
                   [&continue_running, &reissue](
                       const ReissueEvent & /*status*/) noexcept {
                     reissue = true;
                     continue_running = true;
                   },
               },
        execution_status);

    if (continue_running) {
      if (reissue) {
        reissue = false;
        // reuse the same token again
        ASLOG_TO_LOGGER(logger, debug, "re-issuing event({}/{}) as requested ",
            token_type, token_value);
      } else {
        ASAP_ASSERT(token.first != TokenType::EndOfInput);
        token = tokenizer_.NextToken();
      }
    }
  } while (continue_running);
  return no_errors;
}
