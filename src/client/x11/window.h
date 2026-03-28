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

#ifndef AUTK_CLIENT_X11_WINDOW_H_
#define AUTK_CLIENT_X11_WINDOW_H_

#include <autk/window.h>

#include "../../hash.h"

typedef struct autk_x11_window_data {
    xcb_connection_t *connection;
    uint32_t window_id;
} autk_x11_window_data_t;

typedef struct autk_x11_window_map {
    autk_hash_table_t ht;
} autk_x11_window_map_t;

typedef struct autk_x11_window_map_node {
    uint32_t id;
    autk_window_t *window;
} autk_x11_window_map_node_t;

AUTK_HIDDEN extern const autk_window_driver_t autk_window_driver_x11;

AUTK_HIDDEN void
autk_x11_window_invalidate(autk_window_t *window);

AUTK_HIDDEN void
autk_x11_window_map_init(autk_instance_t *instance, autk_x11_window_map_t *map);

AUTK_HIDDEN void
autk_x11_window_map_fini(autk_x11_window_map_t *map);

AUTK_HIDDEN autk_window_t *
autk_x11_window_map_get(autk_x11_window_map_t *map, uint32_t id);

AUTK_HIDDEN autk_status_t
autk_x11_window_map_insert(autk_x11_window_map_t *map, uint32_t id, autk_window_t *window);

AUTK_HIDDEN autk_window_t *
autk_x11_window_map_remove(autk_x11_window_map_t *map, uint32_t id);

#endif // AUTK_CLIENT_X11_WINDOW_H_
