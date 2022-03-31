//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause).
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

/*!
 * \file
 *
 * \brief OptionValue class interface.
 */

#pragma once

#include <clap/asap_clap_api.h>
#include <clap/option_value.h>

#include <string>
#include <unordered_map>

namespace asap::clap {

class OptionValuesMap {
public:
  ASAP_CLAP_API OptionValuesMap() = default;

  OptionValuesMap(const OptionValuesMap &) = delete;
  OptionValuesMap(OptionValuesMap &&) = default;

  auto operator=(const OptionValuesMap &) -> OptionValuesMap & = delete;
  auto operator=(OptionValuesMap &&) -> OptionValuesMap & = delete;

  ASAP_CLAP_API virtual ~OptionValuesMap();

  ASAP_CLAP_API void StoreValue(
      const std::string &option_name, OptionValue new_value);

  [[nodiscard]] ASAP_CLAP_API auto ValuesOf(
      const std::string &option_name) const
      -> const std::vector<OptionValue> & {
    return ovm_.at(option_name);
  }

  [[nodiscard]] ASAP_CLAP_API auto HasOption(
      const std::string &option_name) const -> bool {
    return ovm_.find(option_name) != ovm_.cend();
  }

  [[nodiscard]] ASAP_CLAP_API auto OccurrencesOf(
      const std::string &option_name) const -> size_t {
    auto option = ovm_.find(option_name);
    if (option != ovm_.cend()) {
      return option->second.size();
    }
    return 0;
  }

private:
  std::unordered_map<std::string, std::vector<OptionValue>> ovm_;
};

} // namespace asap::clap
