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

#include <autk/math.h>

AUTK_API void
autk_bbox_extend(autk_bbox_t *bbox, autk_bbox_t add)
{
    if (add.x0 >= add.x1 || add.y0 >= add.y1) {
        return;
    }
    if (bbox->x0 >= bbox->x1 || bbox->y0 >= bbox->y1) {
        *bbox = add;
        return;
    }
    if (add.x0 < bbox->x0) {
        bbox->x0 = add.x0;
    }
    if (add.y0 < bbox->y0) {
        bbox->y0 = add.y0;
    }
    if (add.x1 > bbox->x1) {
        bbox->x1 = add.x1;
    }
    if (add.y1 > bbox->y1) {
        bbox->y1 = add.y1;
    }
}
