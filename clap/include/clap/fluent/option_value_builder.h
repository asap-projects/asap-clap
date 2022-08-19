//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Option value semantics builder fluent API.
 */

#pragma once

#include <memory>

#include <contract/contract.h>

#include "clap/detail/value_descriptor.h"
#include "clap/fluent/option_builder.h"

namespace asap::clap {

template <class T> class OptionValueBuilder : public OptionBuilder {
  using Self = OptionValueBuilder;

public:
  explicit OptionValueBuilder(std::unique_ptr<Option> option)
      : OptionBuilder(std::move(option)),
        value_descriptor_(new ValueDescriptor<T>()) {
    option_->value_semantic_ = value_descriptor_;
  }

  auto StoreTo(T *store_to) -> Self & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->StoreTo(store_to);
    return *this;
  }

  auto DefaultValue(const T &value) -> OptionValueBuilder & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->DefaultValue(value);
    return *this;
  }

  auto DefaultValue(const T &value, const std::string &textual)
      -> OptionValueBuilder & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->DefaultValue(value, textual);
    return *this;
  }

  auto ImplicitValue(const T &value) -> OptionValueBuilder & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->ImplicitValue(value);
    return *this;
  }

  auto ImplicitValue(const T &value, const std::string &textual)
      -> OptionValueBuilder & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->ImplicitValue(value, textual);
    return *this;
  }

  auto Repeatable() -> OptionValueBuilder & {
    ASAP_ASSERT(value_descriptor_ && "builder used after Build() was called");
    value_descriptor_->Repeatable();
    return *this;
  }

private:
  std::shared_ptr<ValueDescriptor<T>> value_descriptor_;
};

// template <>
// inline OptionValueBuilder<bool>::OptionValueBuilder()
//     : option_value_(new ValueDescriptor<bool>(store_to)) {
//   option_value_->DefaultValue(false, "false");
//   option_value_->ImplicitValue(true, "true");
// }

// /**
//  * \brief Make a builder to start describing a new option value.
//  *
//  * This factory method optionally takes as an argument a location
//  * `store_to` which (when not null) will hold the final value(s).
//  *
//  * \see Notifier, Notify
//  */
// template <typename T>
// auto CreateValueDescriptor(T *store_to = nullptr) ->
//     typename ValueDescriptor<T>::Builder {
//   return ValueDescriptor<T>::Builder(store_to);
// }

} // namespace asap::clap
