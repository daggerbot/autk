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

#ifdef _WIN32
# include <windows.h>
#endif

#include <autk/ext/style_ext_base.h>
#include <autk/ext/win9x_style.h>
#include <autk/style.h>

typedef struct autk_win9x_style_data autk_win9x_style_data_t;

struct autk_win9x_style_data {
    autk_win9x_style_color_theme_t color_theme;
    bool explicit_color_theme;
};

#ifdef _WIN32
static inline autk_rgba_t
colorref_to_rgba(COLORREF colorref)
{
    return AUTK_RGB(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
}
#endif

// The default color theme on non-Windows platforms, or if looking up system colors fails.
AUTK_API const autk_win9x_style_color_theme_t autk_win9x_style_color_theme_default = {
    .struct_size = sizeof(autk_win9x_style_color_theme_t),
    .window_background = AUTK_RGB_INIT(0xC0, 0xC0, 0xC0),
};

//==============================================================================
//
// Base extension implementation
//
//==============================================================================

static autk_rgba_t
get_default_window_background_color(const autk_style_t *style, const void *opaque_style_data,
                                    autk_window_type_t window_type)
{
    const autk_win9x_style_data_t *style_data = opaque_style_data;

    (void)style;
    (void)window_type;

    return style_data->color_theme.window_background;
}

static const autk_style_ext_base_v1_t style_ext_base = {
    .header = {.struct_size = sizeof(autk_style_ext_base_v1_t),
               .uuid = AUTK_STYLE_EXTENSION_BASE_INIT,
               .version = 1},
    .get_default_window_background_color = get_default_window_background_color,
};

//==============================================================================
//
// Win9x style implementation
//
//==============================================================================

static const autk_extension_header_t *const win9x_style_extensions[] = {
    &style_ext_base.header,
};

static autk_status_t
win9x_style_init(autk_style_t *style, void *opaque_style_data, const void *class_init_ctx)
{
    autk_win9x_style_data_t *style_data = opaque_style_data;
    const autk_win9x_style_create_params_t *params = class_init_ctx;

    (void)style;

    if (params && params->struct_size != sizeof(autk_win9x_style_create_params_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    // Initialize from an explicit color theme.
    if (params && params->color_theme) {
        if (params->color_theme->struct_size != sizeof(autk_win9x_style_color_theme_t)) {
            return AUTK_ERR_INVALID_STRUCT_SIZE;
        }
        style_data->color_theme = *params->color_theme;
        style_data->explicit_color_theme = true;
        return AUTK_OK;
    }

#ifdef _WIN32
    // Get the system color theme using the legacy GetSysColor() API.
    style_data->color_theme.window_background = colorref_to_rgba(GetSysColor(COLOR_BTNFACE));
    style_data->explicit_color_theme = false;
    return AUTK_OK;
#else
    // Use defaults on non-Windows platforms.
    style_data->color_theme = autk_win9x_style_color_theme_default;
    style_data->explicit_color_theme = true;
    return AUTK_OK;
#endif
}

AUTK_API const autk_style_class_t autk_style_class_win9x = {
    .struct_size = sizeof(autk_style_class_t),
    .class_data_size = sizeof(autk_win9x_style_data_t),
    .extension_count = AUTK_LENGTHOF(win9x_style_extensions),
    .extensions = win9x_style_extensions,
    .init = &win9x_style_init,
};

AUTK_API autk_status_t
autk_win9x_style_create(autk_device_t *device, const autk_win9x_style_create_params_t *params,
                        autk_style_t **out_style)
{
    autk_style_create_params_t style_params = {
        .struct_size = sizeof(autk_style_create_params_t),
        .klass = &autk_style_class_win9x,
        .class_init_ctx = params,
        .user_data_size = params ? params->user_data_size : 0,
        .user_data_init = params ? params->user_data_init : NULL,
    };

    return autk_style_create(device, &style_params, out_style);
}
