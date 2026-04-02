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

#ifndef AUTK_EXT_WIN9X_STYLE_H_
#define AUTK_EXT_WIN9X_STYLE_H_

#include "../types.h"

typedef struct autk_win9x_style_color_theme autk_win9x_style_color_theme_t;
typedef struct autk_win9x_style_create_params autk_win9x_style_create_params_t;

struct autk_win9x_style_color_theme {
    size_t struct_size;
    autk_rgba_t window_background;
};

struct autk_win9x_style_create_params {
    size_t struct_size;
    const autk_win9x_style_color_theme_t *color_theme;
    size_t user_data_size;
    const void *user_data_init;
};

AUTK_API extern const autk_style_class_t autk_style_class_win9x;
AUTK_API extern const autk_win9x_style_color_theme_t autk_win9x_style_color_theme_default;

AUTK_BEGIN_DECLS

AUTK_API autk_status_t
autk_win9x_style_create(autk_instance_t *instance, const autk_win9x_style_create_params_t *params,
                        autk_style_t **out_style);

AUTK_END_DECLS

#endif // AUTK_EXT_WIN9X_STYLE_H_
