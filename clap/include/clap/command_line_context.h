// ===----------------------------------------------------------------------===/
//  Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
//  copy at https://opensource.org/licenses/BSD-3-Clause).
//  SPDX-License-Identifier: BSD-3-Clause
// ===----------------------------------------------------------------------===/

/*!
 * \file
 *
 * \brief Command line context, encapsulating the command line io streams, .
 */

#pragma once

#include <clap/option_values_map.h>

#include <iostream>

namespace asap::clap {

struct CommandLineContext {
  explicit CommandLineContext(std::string program_name, OptionValuesMap &ovm)
      : program_name_{std::move(program_name)}, ovm_{ovm} {
  }

  bool allow_long_option_value_with_no_equal{true};

  std::istream &in_{std::cin};
  std::ostream &out_{std::cout};
  std::ostream &err_{std::cerr};

  std::string program_name_;

  OptionValuesMap &ovm_;
};

} // namespace asap::clap
