//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for OptionValue.
 */

#include "clap/option_value.h"

// This is only here so that the vtable of OptionValue does not get emitted in
// every translation unit. (see clang warning 'weak-vtables').
// https://stackoverflow.com/a/23749273
asap::clap::OptionValue::~OptionValue() = default;
