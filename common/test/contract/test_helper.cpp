/*     SPDX-License-Identifier: BSD-3-Clause     */

//        Copyright The Authors 2021.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include "contract/contract.h"

#include "test_helper.h"

namespace asap {
namespace contract {
namespace testing {

auto TestExpectDefault(const int *ptr) -> int {
  ASAP_EXPECT(ptr);
  return *ptr;
}

auto TestEnsureDefault(const int *ptr) -> int {
  ASAP_ENSURE(ptr);
  return *ptr;
}

auto TestAssertDefault(const int *ptr) -> int {
  ASAP_ASSERT(ptr);
  return *ptr;
}

auto TestExpectAudit(const int *ptr) -> int {
  ASAP_EXPECT_AUDIT(ptr);
  return *ptr;
}

auto TestEnsureAudit(const int *ptr) -> int {
  ASAP_ENSURE_AUDIT(ptr);
  return *ptr;
}

auto TestAssertAudit(const int *ptr) -> int {
  ASAP_ASSERT_AUDIT(ptr);
  return *ptr;
}

auto TestExpectAxiom(const int *ptr) -> int {
  ASAP_EXPECT_AXIOM(ptr);
  return *ptr;
}

auto TestEnsureAxiom(const int *ptr) -> int {
  ASAP_ENSURE_AXIOM(ptr);
  return *ptr;
}

auto TestAssertAxiom(const int *ptr) -> int {
  ASAP_ASSERT_AXIOM(ptr);
  return *ptr;
}

} // namespace testing
} // namespace contract
} // namespace asap
