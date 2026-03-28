/*
 * Copyright (c) 2026 Martin Mills
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef AUTK_IMPL_MATH_H_
#define AUTK_IMPL_MATH_H_

#include <autk/types.h>

AUTK_BEGIN_DECLS

#define AUTK_DEFINE_INT_MATH(type, name)                                                           \
    static inline type autk_##name##_clamp(type n, type min, type max)                             \
    {                                                                                              \
        return n < min ? min : (n > max ? max : n);                                                \
    }                                                                                              \
                                                                                                   \
    static inline type autk_##name##_max(type a, type b)                                           \
    {                                                                                              \
        return a > b ? a : b;                                                                      \
    }                                                                                              \
                                                                                                   \
    static inline type autk_##name##_min(type a, type b)                                           \
    {                                                                                              \
        return a < b ? a : b;                                                                      \
    }

AUTK_DEFINE_INT_MATH(int32_t, int32)
AUTK_DEFINE_INT_MATH(uint32_t, uint32)

static inline size_t
autk_align_up(size_t n)
{
#ifdef AUTK_ALIGNOF
    return ((n - 1) | (AUTK_ALIGNOF(max_align_t) - 1)) + 1;
#else
    return ((n - 1) | 15) + 1;
#endif
}

AUTK_END_DECLS

#endif // AUTK_IMPL_MATH_H_
