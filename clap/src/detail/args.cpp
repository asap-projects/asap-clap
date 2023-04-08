//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief Implementation details for the Arguments class.
 */

#include <gsl/gsl>

#include <contract/contract.h>

#include "clap/detail/args.h"

class asap::clap::detail::Arguments::ArgumentsImpl {
public:
  ArgumentsImpl(int argc, const char **argv) {
    ASAP_EXPECT(argc > 0);
    ASAP_EXPECT(argv != nullptr);

    auto s_argv = gsl::span(argv, static_cast<size_t>(argc));
    // Extract the program name from the first argument (should always be there)
    // and keep the rest of the arguments for later parsing
    program_name.assign(s_argv[0]);
    if (argc > 1) {
      args.assign(++s_argv.begin(), s_argv.end());
    }

    ASAP_ENSURE(!program_name.empty());
  }

  std::string program_name;
  std::vector<std::string> args{};
};

asap::clap::detail::Arguments::Arguments(int argc, const char **argv)
    : impl_(new ArgumentsImpl(argc, argv),
          // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
          [](const ArgumentsImpl *impl) { delete impl; }) {
}

auto asap::clap::detail::Arguments::ProgramName() const -> const std::string & {
  return impl_->program_name;
}
auto asap::clap::detail::Arguments::Args() const -> std::vector<std::string> & {
  return impl_->args;
}
