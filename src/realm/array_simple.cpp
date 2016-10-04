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

#include <realm/array_simple.hpp>
#include <realm/array_direct.hpp>


namespace {

inline int_fast64_t lbound_for_width(unsigned width)
{
    if (width < 8) {
        return 0;
    }
    width--;
    return 0 - (1LL << width);
}

inline int_fast64_t ubound_for_width(unsigned width) noexcept
{
    if (width >= 8)
        width--;
    return (1LL << width) - 1;
}

} // namespace

namespace realm {

SimpleArray::SimpleArray(Allocator& alloc)
    : DatabaseElement(alloc)
{
}

void SimpleArray::create(Type type, bool context_flag)
{
    MemRef mem = create_element(0, get_alloc(), context_flag, type, wtype_Bits, 0); // Throws
    init_from_mem(mem);
}

void SimpleArray::insert(size_t ndx, int_fast64_t value)
{
    bool do_expand = value < m_lbound || value > m_ubound;
    uint_least8_t new_width = do_expand ? bit_width(value) : m_width;

    REALM_ASSERT_DEBUG(!do_expand || new_width > m_width);
    REALM_ASSERT_DEBUG(ndx <= m_size);

    // Check if we need to copy before modifying
    copy_on_write();              // Throws
    alloc(m_size + 1, new_width); // Throws

    // Move values above insertion (may expand)
    if (do_expand || new_width < 8) {
        size_t i = m_size;
        while (i > ndx) {
            --i;
            int64_t v = get_direct(m_data, m_width, i);
            set_direct(m_data, new_width, i + 1, v);
        }
    }
    else if (ndx != m_size) {
        // when byte sized and no expansion, use memmove
        size_t w = (new_width >> 3);
        char* src_begin = m_data + ndx * w;
        char* src_end = m_data + m_size * w;
        char* dst_end = src_end + w;
        std::copy_backward(src_begin, src_end, dst_end);
    }

    // Insert the new value
    set_direct(m_data, new_width, ndx, value);

    // Expand values before insertion
    if (do_expand) {
        size_t i = ndx;
        while (i != 0) {
            --i;
            int64_t v = get_direct(m_data, m_width, i);
            set_direct(m_data, new_width, i, v);
        }
        set_width(new_width);
    }

    // Update size
    // (no need to do it in header as it has been done by Alloc)
    ++m_size;
}

void SimpleArray::erase(size_t ndx)
{
    copy_on_write(); // Throws

    for (size_t i = ndx; i < size() - 1; i++) {
        set(i, get(i + 1));
    }
    // Update size (also in header)
    --m_size;
    set_header_size(m_size);
}

int64_t SimpleArray::get(size_t index) const
{
    return get_direct(m_data, m_width, index);
}

void SimpleArray::set(size_t ndx, int_fast64_t value)
{
    set_direct(m_data, m_width, ndx, value);
}

void SimpleArray::clear()
{
    truncate(0);
    set_width(0);
    set_header_width(0);
}

void SimpleArray::set_width(uint_least8_t width)
{
    m_lbound = lbound_for_width(width);
    m_ubound = ubound_for_width(width);
    m_width = width;
}

} // namespace realm
