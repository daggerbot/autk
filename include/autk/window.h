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

#ifndef AUTK_WINDOW_H_
#define AUTK_WINDOW_H_

#include "types.h"

/// \see \ref autk_window_create_flags
typedef uint32_t autk_window_create_flags_t;
enum autk_window_create_flags {
    /// By default, if `x` and `y` are both zero, the window will be created at a default position
    /// at the window system's discretion. Setting this flag forces the window to be created at the
    /// specified `x` and `y` coordinates even if they are zero.
    AUTK_WINDOW_CREATE_FLAGS_EXPLICIT_POSITION = 1 << 0,

    AUTK_WINDOW_CREATE_FLAGS_ALL = AUTK_WINDOW_CREATE_FLAGS_EXPLICIT_POSITION,
    AUTK_WINDOW_CREATE_FLAGS_32BIT_ = 0x7FFFFFFF,
};

typedef enum autk_window_type {
    AUTK_WINDOW_TYPE_NORMAL,
} autk_window_type_t;

typedef struct autk_window_callbacks autk_window_callbacks_t;
typedef struct autk_window_create_params autk_window_create_params_t;
typedef struct autk_window_driver autk_window_driver_t;

struct autk_window_callbacks {
    size_t struct_size;
    /// Invoked when the window is destroyed via \ref autk_window_destroy.
    void (*destroying)(autk_window_t *window, void *user_data);
    /// Invoked when a window destroy event is received from the window system without a
    /// corresponding \ref autk_window_destroy call.
    void (*invalidated)(autk_window_t *window, void *user_data);
    /// Invoked when the user or window system sends a window close request.
    void (*close_requested)(autk_window_t *window, void *user_data);
};

struct autk_window_create_params {
    size_t struct_size;
    autk_window_type_t type;
    autk_window_create_flags_t flags;
    const char *title;
    int32_t x, y;
    uint32_t width, height;
    const autk_window_callbacks_t *callbacks;
    size_t user_data_size;
    const void *user_data_init;
};

struct autk_window_driver {
    size_t struct_size;
    size_t driver_data_size;

    autk_status_t (*init)(autk_window_t *window, void *driver_data,
                          const autk_window_create_params_t *params);
    void (*fini)(autk_window_t *window, void *driver_data);
    autk_status_t (*set_title)(autk_window_t *window, void *driver_data, const char *title);
    autk_status_t (*set_visible)(autk_window_t *window, void *driver_data, bool visible);
};

AUTK_BEGIN_DECLS

AUTK_API autk_status_t
autk_window_create(autk_client_t *client, const autk_window_create_params_t *params,
                   autk_window_t **out_window);

AUTK_API void
autk_window_destroy(autk_window_t *window);

AUTK_API autk_instance_t *
autk_window_get_instance(const autk_window_t *window);

AUTK_API autk_client_t *
autk_window_get_client(const autk_window_t *window);

AUTK_API const autk_window_driver_t *
autk_window_get_driver(autk_window_t *window);

AUTK_API void *
autk_window_get_driver_data(autk_window_t *window);

AUTK_API void *
autk_window_get_user_data(autk_window_t *window);

AUTK_API autk_status_t
autk_window_set_title(autk_window_t *window, const char *title);

AUTK_API autk_status_t
autk_window_set_visible(autk_window_t *window, bool visible);

// Helper callbacks

AUTK_API void
autk_window_callback_destroy(autk_window_t *window, void *unused);

AUTK_API void
autk_window_callback_quit(autk_window_t *window, void *unused);

AUTK_END_DECLS

#endif // AUTK_WINDOW_H_
