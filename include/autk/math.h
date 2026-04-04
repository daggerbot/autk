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

#ifndef AUTK_MATH_H_
#define AUTK_MATH_H_

#include "types.h"

AUTK_BEGIN_DECLS

AUTK_API void
autk_bbox_extend(autk_bbox_t *bbox, autk_bbox_t add);

static inline bool
autk_bbox_is_positive(const autk_bbox_t *bbox)
{
    return bbox->x0 < bbox->x1 && bbox->y0 < bbox->y1;
}

static inline int32_t
autk_scale_int32(int32_t n, autk_u30x2_t scale)
{
    return (int32_t)((n * (int64_t)scale) >> 2);
}

static inline uint32_t
autk_scale_uint32(uint32_t n, autk_u30x2_t scale)
{
    return (uint32_t)((n * (uint64_t)scale) >> 2);
}

static inline bool
autk_uuid_equals(const autk_uuid_t *a, const autk_uuid_t *b)
{
    return a->parts[0] == b->parts[0] && a->parts[1] == b->parts[1];
}

static inline bool
autk_uuid_is_zero(const autk_uuid_t *uuid)
{
    return uuid->parts[0] == 0 && uuid->parts[1] == 0;
}

AUTK_END_DECLS

#endif // AUTK_MATH_H_
