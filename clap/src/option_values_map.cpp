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

#include "clap/option_values_map.h"

// This is only here so that the vtable of OptionValue does not get emitted in
// every translation unit. (see clang warning 'weak-vtables').
// https://stackoverflow.com/a/23749273
asap::clap::OptionValuesMap::~OptionValuesMap() = default;

void asap::clap::OptionValuesMap::StoreValue(
    const std::string &option_name, OptionValue new_value) {
  auto in_ovm = ovm_.find(option_name);
  if (in_ovm != ovm_.end()) {
    auto &option_values = in_ovm->second;
    option_values.emplace_back(std::move(new_value));
    return;
  }
  ovm_.emplace(option_name, std::vector<OptionValue>{std::move(new_value)});
}
