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

#ifndef AUTK_EXT_STYLE_EXT_BASE_H_
#define AUTK_EXT_STYLE_EXT_BASE_H_

#include "../types.h"

// {019d4c23-7a3a-75e1-8f77-8260daa391c8}
#define AUTK_STYLE_EXTENSION_BASE_INIT                                                             \
    {.bytes = {0x01, 0x9d, 0x4c, 0x23, 0x7a, 0x3a, 0x75, 0xe1, 0x8f, 0x77, 0x82, 0x60, 0xda, 0xa3, \
               0x91, 0xc8}}
#define AUTK_STYLE_EXTENSION_BASE ((autk_uuid_t)AUTK_STYLE_EXTENSION_BASE_INIT)

typedef struct autk_style_ext_base_v1 {
    autk_extension_header_t header;
    autk_rgba_t (*get_default_window_background_color)(const autk_style_t *style,
                                                       const void *style_data,
                                                       autk_window_type_t window_type);
} autk_style_ext_base_v1_t;

AUTK_BEGIN_DECLS

AUTK_API autk_rgba_t
autk_style_ext_base_get_default_window_background_color(const autk_style_t *style,
                                                        const autk_extension_header_t *ext,
                                                        autk_window_type_t window_type);

AUTK_END_DECLS

#endif // AUTK_EXT_STYLE_EXT_BASE_H_
