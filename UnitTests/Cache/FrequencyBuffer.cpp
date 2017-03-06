////////////////////////////////////////////////////////////////////////////////
/// @brief test suite for arangodb::cache::FrequencyBuffer
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2017 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Daniel H. Larkin
/// @author Copyright 2017, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "Basics/Common.h"

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

#include "Cache/FrequencyBuffer.h"

#include <stdint.h>
#include <memory>

#include <iostream>

using namespace arangodb::cache;

// -----------------------------------------------------------------------------
// --SECTION--                                                 setup / tear-down
// -----------------------------------------------------------------------------

struct CCacheFrequencyBufferSetup {
  CCacheFrequencyBufferSetup() { BOOST_TEST_MESSAGE("setup FrequencyBuffer"); }

  ~CCacheFrequencyBufferSetup() {
    BOOST_TEST_MESSAGE("tear-down FrequencyBuffer");
  }
};
// -----------------------------------------------------------------------------
// --SECTION--                                                        test suite
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief setup
////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE(CCacheFrequencyBufferTest, CCacheFrequencyBufferSetup)

////////////////////////////////////////////////////////////////////////////////
/// @brief test behavior with ints
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(tst_uint8_t) {
  uint8_t zero = 0;
  uint8_t one = 1;
  uint8_t two = 2;

  // check that default construction is as expected
  BOOST_CHECK(uint8_t() == zero);

  FrequencyBuffer<uint8_t> buffer(8);
  BOOST_CHECK_EQUAL(
      buffer.memoryUsage(),
      sizeof(FrequencyBuffer<uint8_t>) + sizeof(std::vector<uint8_t>) + 8);

  for (size_t i = 0; i < 4; i++) {
    buffer.insertRecord(two);
  }
  for (size_t i = 0; i < 2; i++) {
    buffer.insertRecord(one);
  }

  auto frequencies = buffer.getFrequencies();
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(2), frequencies->size());
  BOOST_CHECK_EQUAL(one, (*frequencies)[0].first);
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(2), (*frequencies)[0].second);
  BOOST_CHECK_EQUAL(two, (*frequencies)[1].first);
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(4), (*frequencies)[1].second);

  for (size_t i = 0; i < 8; i++) {
    buffer.insertRecord(one);
  }

  frequencies = buffer.getFrequencies();
  BOOST_CHECK_EQUAL(static_cast<size_t>(1), frequencies->size());
  BOOST_CHECK_EQUAL(one, (*frequencies)[0].first);
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(8), (*frequencies)[0].second);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test behavior with shared_ptr
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(tst_pointers) {
  struct cmp_weak_ptr {
    bool operator()(std::weak_ptr<int> const& left,
                    std::weak_ptr<int> const& right) const {
      return !left.owner_before(right) && !right.owner_before(left);
    }
  };

  struct hash_weak_ptr {
    size_t operator()(std::weak_ptr<int> const& wp) const {
      auto sp = wp.lock();
      return std::hash<decltype(sp)>()(sp);
    }
  };

  typedef FrequencyBuffer<std::weak_ptr<int>, cmp_weak_ptr, hash_weak_ptr>
      BufferType;

  std::shared_ptr<int> p0(nullptr);

  // check that default construction is as expected
  BOOST_CHECK(std::shared_ptr<int>() == p0);

  std::shared_ptr<int> p1(new int());
  *p1 = static_cast<int>(1);
  std::shared_ptr<int> p2(new int());
  *p2 = static_cast<int>(2);

  BufferType buffer(8);
  BOOST_CHECK_EQUAL(buffer.memoryUsage(),
                    sizeof(BufferType) +
                        sizeof(std::vector<std::weak_ptr<int>>) +
                        (8 * sizeof(std::weak_ptr<int>)));

  for (size_t i = 0; i < 4; i++) {
    buffer.insertRecord(p1);
  }
  for (size_t i = 0; i < 2; i++) {
    buffer.insertRecord(p2);
  }

  auto frequencies = buffer.getFrequencies();
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(2), frequencies->size());
  BOOST_CHECK(p2 == (*frequencies)[0].first.lock());
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(2), (*frequencies)[0].second);
  BOOST_CHECK(p1 == (*frequencies)[1].first.lock());
  BOOST_CHECK_EQUAL(static_cast<uint64_t>(4), (*frequencies)[1].second);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief generate tests
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|//
// --SECTION--\\|/// @\\}\\)"
// End:
