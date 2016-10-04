/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#ifndef REALM_ARRAY_SIMPLE_HPP_
#define REALM_ARRAY_SIMPLE_HPP_

#include <realm/database_element.hpp>

namespace realm {

class SimpleArray : public DatabaseElement {

public:
    SimpleArray(Allocator& alloc);

    void create(Type = type_Normal, bool context_flag = false);
    void init_from_ref(ref_type ref) noexcept
    {
        REALM_ASSERT_DEBUG(ref);
        char* header = m_alloc.translate(ref);
        init_from_mem(MemRef(header, ref, m_alloc));
    }
    void init_from_mem(MemRef mem) noexcept
    {
        DatabaseElement::init_from_mem(mem);
        set_width(m_width);
    }

    void add(int_fast64_t value)
    {
        insert(m_size, value);
    }
    //  insert  value at  index (moving successive  elements  1 position  forwards)
    void insert(size_t ndx, int_fast64_t value);
    // delete value at index
    void erase(size_t ndx);
    //  return  value at  index
    int64_t get(size_t index) const;
    // override value at index
    void set(size_t ndx, int_fast64_t value);

    void clear();

private:
    int64_t m_lbound; // min number that can be stored with current m_width
    int64_t m_ubound; // max number that can be stored with current m_width

    void set_width(uint_least8_t width);
};

} // namespace

#endif /* SRC_REALM_ARRAY_SIMPLE_HPP_ */
