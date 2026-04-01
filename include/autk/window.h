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

#define AUTK_WINDOW_FALLBACK_WIDTH 640
#define AUTK_WINDOW_FALLBACK_HEIGHT 480

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
