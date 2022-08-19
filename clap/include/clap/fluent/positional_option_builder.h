//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Positional option builder fluent API.
 */

#pragma once

#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>

#include "clap/fluent/option_value_builder.h"

namespace asap::clap {

template <typename Builder,
    std::enable_if_t<std::is_same_v<Builder, OptionBuilder>, std::nullptr_t>>
class Positional : Builder {
public:
  explicit Positional(std::string name)
      : Builder(std::forward<std::string>(name)) {
  }

  auto About(std::string about) -> Positional<Builder> & {
    Builder::About(std::forward<std::string>(about));
    return *this;
  }

  template <typename T>
  auto WithValue(typename ValueDescriptor<T>::Builder &option_value_builder)
      -> Positional<Builder> & {
    Builder::template WithValue<T>(option_value_builder);
    return *this;
  }

  template <typename T>
  auto WithValue(typename ValueDescriptor<T>::Builder &&option_value_builder)
      -> Positional<Builder> & {
    Builder::template WithValue<T>(option_value_builder);
    return *this;
  }

  using Builder::Build;
  using Builder::WithValue;
};

} // namespace asap::clap
