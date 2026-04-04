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

#ifndef AUTK_CLIENT_X11_TYPES_H_
#define AUTK_CLIENT_X11_TYPES_H_

#include <xcb/xcb.h>

#include <core/types.h>
#include <os/posix/job_queue.h>
#include <utility/hash.h>

typedef struct autk_x11_atoms autk_x11_atoms_t;
typedef struct autk_x11_client_data autk_x11_client_data_t;
typedef struct autk_x11_window_data autk_x11_window_data_t;
typedef struct autk_x11_window_map autk_x11_window_map_t;
typedef struct autk_x11_window_map_node autk_x11_window_map_node_t;

struct autk_x11_atoms {
    /* clang-format off */
#define AUTK_FOR_EACH_X11_ATOM(m) \
    m(UTF8_STRING) \
    m(WM_DELETE_WINDOW) \
    m(WM_PROTOCOLS) \
    m(_NET_WM_ICON_NAME) \
    m(_NET_WM_NAME)
/* clang-format on */
#define AUTK_DO(name) uint32_t atom_##name;
    AUTK_FOR_EACH_X11_ATOM(AUTK_DO)
#undef AUTK_DO
};

struct autk_x11_window_data {
    xcb_connection_t *connection;
    uint32_t window_id;
    autk_bbox_t dirty_region;
};

struct autk_x11_window_map {
    autk_hash_table_t ht;
};

struct autk_x11_window_map_node {
    uint32_t id;
    autk_window_t *window;
};

struct autk_x11_client_data {
    xcb_connection_t *connection;
    int default_screen_num;
    int display_fd;
    xcb_screen_t *default_screen;
    uint8_t default_depth;
    xcb_visualtype_t *default_visual;
    autk_rgb_t color_shift;
    uint32_t default_colormap;
    autk_x11_atoms_t atoms;
    autk_x11_window_map_t window_map;
    autk_posix_job_queue_t job_queue;
    bool quit_requested;
};

#endif // AUTK_CLIENT_X11_TYPES_H_
