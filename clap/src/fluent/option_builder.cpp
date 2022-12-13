//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for the option builder fluent API.
 */

#include <contract/contract.h>

#include "clap/fluent/option_builder.h"
#include "clap/fluent/option_value_builder.h"

auto asap::clap::OptionBuilder::Short(std::string short_name) -> Self & {
  ASAP_ASSERT(option_ && "builder used after Build() was called");
  option_->Short(std::move(short_name));
  return *this;
}

auto asap::clap::OptionBuilder::Long(std::string long_name) -> Self & {
  ASAP_ASSERT(option_ && "builder used after Build() was called");
  option_->Long(std::move(long_name));
  return *this;
}

auto asap::clap::OptionBuilder::About(std::string about) -> Self & {
  ASAP_ASSERT(option_ && "builder used after Build() was called");
  option_->About(std::move(about));
  return *this;
}

template <>
auto asap::clap::OptionBuilder::WithValue<bool>() -> OptionValueBuilder<bool> {
  OptionValueBuilder<bool> value_builder(std::move(option_));
  value_builder.DefaultValue(false, "false");
  value_builder.ImplicitValue(true, "true");
  return value_builder;
}
