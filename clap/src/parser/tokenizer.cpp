//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details of the command line arguments parsing
 * tokenizer.
 */

#include "tokenizer.h"

#include <fsm/fsm.h>

#include <utility>

using asap::fsm::ByDefault;
using asap::fsm::Continue;
using asap::fsm::DoNothing;
using asap::fsm::Maybe;
using asap::fsm::On;
using asap::fsm::OneOf;
using asap::fsm::ReissueEvent;
using asap::fsm::StateMachine;
using asap::fsm::Status;
using asap::fsm::Terminate;
using asap::fsm::TransitionTo;
using asap::fsm::Will;

namespace asap::clap::parser {

auto operator<<(std::ostream &out, const TokenType &token_type)
    -> std::ostream & {
  out << magic_enum::enum_name(token_type);
  return out;
}

namespace {

struct InputChar {
  explicit InputChar(char character) : value{character} {
  }

  const char value;
};
struct InputEnd {};

using TokenConsumer =
    std::function<void(TokenType token_type, std::string token)>;

struct InitialState;
struct ValueState;
struct OptionState;
struct ShortOptionState;
struct LongOptionState;
struct DashDashState;
struct FinalState;

struct FinalState : public Will<ByDefault<DoNothing>> {
  static auto OnEnter(const InputEnd & /*event*/) -> Status {
    //    std::cout << "InputEnd -> FinalState" << std::endl;
    return Continue{};
  }
};

struct InitialState : public ByDefault<TransitionTo<FinalState>> {
  using ByDefault::Handle;

  template <typename Event> auto OnLeave(const Event & /*event*/) -> Status {
    //    std::cout << "InitialState -> " << std::endl;
    return Continue{};
  }

  static auto Handle(const InputChar &event)
      -> OneOf<TransitionTo<ValueState>, TransitionTo<OptionState>> {
    switch (event.value) {
    case '-':
      return TransitionTo<OptionState>{};
    default:
      return TransitionTo<ValueState>{};
    }
  }
};

struct ValueState : public On<InputEnd, TransitionTo<FinalState>> {
  using On::Handle;

  explicit ValueState(TokenConsumer callback)
      : consume_token_{std::move(callback)} {
  }

  auto OnEnter(const InputChar &event) -> Status {
    //    std::cout << "InputChar(" << event.value << ") -> ValueState" <<
    //    std::endl;
    token_.push_back((event.value));
    return Continue{};
  }

  auto OnLeave(const InputEnd & /*event*/) -> Status {
    //    std::cout << "ValueState -> " << std::endl;
    consume_token_(TokenType::Value, token_);
    return Continue{};
  }

  auto Handle(const InputChar &event) -> DoNothing {
    token_.push_back(event.value);
    return DoNothing{};
  }

private:
  std::string token_;
  TokenConsumer consume_token_;
};

struct OptionState {

  explicit OptionState(TokenConsumer callback)
      : consume_token_{std::move(callback)} {
  }

  static auto OnEnter(const InputChar & /*event*/) -> Status {
    //    std::cout << "InputChar(" << event.value << ") -> OptionState" <<
    //    std::endl;
    return Continue{};
  }

  template <typename Event> auto OnLeave(const Event & /*event*/) -> Status {
    //    std::cout << "OptionState -> " << std::endl;
    return Continue{};
  }

  auto Handle([[maybe_unused]] const InputEnd &event)
      -> TransitionTo<FinalState> {
    consume_token_(TokenType::LoneDash, "-");
    return TransitionTo<FinalState>{};
  }

  static auto Handle(const InputChar &event)
      -> OneOf<TransitionTo<DashDashState>, TransitionTo<ShortOptionState>> {
    switch (event.value) {
    case '-':
      return TransitionTo<DashDashState>{};
    default:
      return TransitionTo<ShortOptionState>{};
    }
  }

private:
  TokenConsumer consume_token_;
};

struct ShortOptionState : public On<InputEnd, TransitionTo<FinalState>> {
  using On::Handle;

  explicit ShortOptionState(TokenConsumer callback)
      : consume_token_{std::move(callback)} {
  }

  auto OnEnter(const InputChar &event) -> Status {
    //    std::cout << "InputChar(" << event.value << ") -> ShortOptionState" <<
    //    std::endl;
    consume_token_(TokenType::ShortOption, std::string{event.value});
    return Continue{};
  }

  template <typename Event> auto OnLeave(const Event & /*event*/) -> Status {
    //    std::cout << "ShortOptionState -> " << std::endl;
    return Continue{};
  }

  auto Handle(const InputChar &event) -> DoNothing {
    consume_token_(TokenType::ShortOption, std::string{event.value});
    return DoNothing{};
  }

private:
  TokenConsumer consume_token_;
};

struct DashDashState : On<InputChar, TransitionTo<LongOptionState>> {
  using On::Handle;

  explicit DashDashState(TokenConsumer callback)
      : consume_token_{std::move(callback)} {
  }

  static auto OnEnter(const InputChar & /*event*/) -> Status {
    //    std::cout << "InputChar(" << event.value << ") -> DashDashState" <<
    //    std::endl;
    return Continue{};
  }

  template <typename Event> auto OnLeave(const Event & /*event*/) -> Status {
    //    std::cout << "DashDashState -> " << std::endl;
    return Continue{};
  }

  auto Handle([[maybe_unused]] const InputEnd &event)
      -> TransitionTo<FinalState> {
    consume_token_(TokenType::DashDash, "--");
    return TransitionTo<FinalState>{};
  }

private:
  TokenConsumer consume_token_;
};

struct LongOptionState : public On<InputEnd, TransitionTo<FinalState>> {
  using On::Handle;

  explicit LongOptionState(TokenConsumer callback)
      : consume_token_{std::move(callback)} {
  }

  auto OnEnter(const InputChar &event) -> Status {
    //    std::cout << "InputChar(" << event.value << ") -> LongOptionState" <<
    //    std::endl;
    token_.push_back((event.value));
    return Continue{};
  }

  auto OnLeave(const InputEnd & /*event*/) -> Status {
    //    std::cout << "LongOptionState -> " << std::endl;
    consume_token_(TokenType::LongOption, token_);
    return Continue{};
  }

  auto OnLeave(const InputChar & /*event*/) -> Status {
    //    std::cout << "LongOptionState -> " << std::endl;
    after_equal_sign = false;
    return Continue{};
  }

  auto Handle(const InputChar &event) -> Maybe<TransitionTo<ValueState>> {
    switch (event.value) {
    case '=':
      consume_token_(TokenType::LongOption, token_);
      consume_token_(TokenType::EqualSign, "=");
      after_equal_sign = true;
      break;
    default:
      if (after_equal_sign) {
        return TransitionTo<ValueState>{};
      }
      token_.push_back(event.value);
    }
    return DoNothing{};
  }

private:
  std::string token_;
  bool after_equal_sign{false};
  TokenConsumer consume_token_;
};

} // namespace

void Tokenizer::Tokenize(const std::string &arg) const {

  TokenConsumer consume_token = [this](TokenType token_type,
                                    std::string token) -> void {
    //    std::cout << "New token: " << token_type << " / " << token <<
    //    std::endl;
    this->tokens_.emplace_back(token_type, std::move(token));
  };

  StateMachine<InitialState, ValueState, OptionState, ShortOptionState,
      DashDashState, LongOptionState, FinalState>
      machine{InitialState{}, ValueState{consume_token},
          OptionState{consume_token}, ShortOptionState{consume_token},
          DashDashState{consume_token}, LongOptionState{consume_token},
          FinalState{}};

  auto cursor = arg.begin();
  while (cursor != arg.end()) {
    machine.Handle(InputChar{*cursor});
    cursor++;
  }
  machine.Handle(InputEnd{});
}

} // namespace asap::clap::parser
