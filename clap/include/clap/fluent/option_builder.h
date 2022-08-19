//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Option builder fluent API.
 */

#pragma once

#include <memory>
#include <string>
#include <utility>

#include "clap/asap_clap_api.h"
#include "clap/option.h"

namespace asap::clap {

template <typename T> class OptionValueBuilder;

class OptionBuilder {
  using Self = OptionBuilder;

public:
  explicit OptionBuilder(std::string name)
      : option_(new Option(std::move(name))) {
  }

  ASAP_CLAP_API auto Short(std::string short_name) -> OptionBuilder &;

  ASAP_CLAP_API auto Long(std::string long_name) -> OptionBuilder &;

  ASAP_CLAP_API auto About(std::string about) -> OptionBuilder &;

  // template <typename T>
  // auto WithValue(typename ValueDescriptor<T>::Builder &option_value_builder)
  //     -> OptionBuilder & {
  //   ASAP_ASSERT(option_ && "builder used after Build() was called");
  //   option_->value_semantic_ = std::move(option_value_builder.Build());
  //   return *this;
  // }

  // template <typename T>
  // auto WithValue(typename ValueDescriptor<T>::Builder &&option_value_builder)
  //     -> OptionBuilder & {
  //   ASAP_ASSERT(option_ && "builder used after Build() was called");
  //   option_->value_semantic_ = std::move(option_value_builder.Build());
  //   return *this;
  // }

  auto Build() -> std::shared_ptr<Option> {
    return std::move(option_);
  }

  // Builder facets

  template <typename T> auto WithValue() -> OptionValueBuilder<T> {
    return OptionValueBuilder<T>(std::move(option_));
  }

protected:
  // We use a `unique_ptr` here instead of a simple contained object in order to
  // have an option initialized at construction of the builder, and then once
  // moved out, the builder becomes explicitly invalid.
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
  std::unique_ptr<Option> option_;

  explicit OptionBuilder(std::unique_ptr<Option> option)
      : option_{std::move(option)} {
  }
};

template <>
ASAP_CLAP_API auto asap::clap::OptionBuilder::WithValue<bool>()
    -> OptionValueBuilder<bool>;

} // namespace asap::clap
