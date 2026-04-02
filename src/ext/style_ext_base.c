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

#include <autk/ext/style_ext_base.h>
#include <core/types.h>

AUTK_API autk_rgba_t
autk_style_ext_base_get_default_window_background_color(const autk_style_t *style,
                                                        const autk_extension_header_t *ext_header,
                                                        autk_window_type_t window_type)
{
    autk_style_ext_base_v1_t *ext = (autk_style_ext_base_v1_t *)ext_header;

    if (ext->get_default_window_background_color) {
        return ext->get_default_window_background_color(style, style->class_data, window_type);
    } else {
        return (autk_rgba_t){0};
    }
}
