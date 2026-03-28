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

#include <assert.h>
#include <inttypes.h>

#include <xcb/xcb.h>

#include <autk/diagnostics.h>

#include "../../hash.h"
#include "../../impl_math.h"
#include "client.h"
#include "window.h"

//==============================================================================
//
// Public X11 window functions
//
//==============================================================================

AUTK_HIDDEN void
autk_x11_window_invalidate(autk_window_t *window)
{
    autk_x11_window_data_t *window_data = window->driver_data;
    autk_x11_client_data_t *client_data;
    autk_window_t *removed_window;

    if (!window) {
        return;
    }

    assert(window->driver == &autk_window_driver_x11);
    client_data = window->client->driver_data;

    if (window_data->window_id != 0) {
        removed_window =
            autk_x11_window_map_remove(&client_data->window_map, window_data->window_id);
        assert(removed_window == window);
        window_data->window_id = 0;
    }

    if (window->callbacks && window->callbacks->invalidated) {
        window->callbacks->invalidated(window, window->user_data);
    }
}

//==============================================================================
//
// X11 window driver
//
//==============================================================================

static autk_status_t
set_wm_protocols(autk_x11_client_data_t *client_data, autk_x11_window_data_t *window_data)
{
    uint32_t protocols[] = {client_data->atoms.atom_WM_DELETE_WINDOW};

    xcb_change_property(client_data->connection, XCB_PROP_MODE_REPLACE, window_data->window_id,
                        client_data->atoms.atom_WM_PROTOCOLS, XCB_ATOM_ATOM, 32,
                        sizeof(protocols) / sizeof(protocols[0]), protocols);

    return AUTK_OK;
}

static autk_status_t
autk_x11_window_init(autk_window_t *window, void *opaque_driver_data,
                     const autk_window_create_params_t *params)
{
    static const uint32_t value_list[] = {
        XCB_EVENT_MASK_STRUCTURE_NOTIFY,
    };

    autk_x11_window_data_t *window_data = opaque_driver_data;
    autk_x11_client_data_t *client_data = window->client->driver_data;

    window_data->connection = client_data->connection;

    // Claim a new ID for the window.
    window_data->window_id = xcb_generate_id(client_data->connection);

    // Create the window.
    xcb_create_window(window_data->connection, client_data->default_depth, window_data->window_id,
                      client_data->default_screen->root,
                      (int16_t)autk_int32_clamp(params->x, INT16_MIN, INT16_MAX),
                      (int16_t)autk_int32_clamp(params->y, INT16_MIN, INT16_MAX),
                      (uint16_t)autk_uint32_min(params->width ? params->width : 100, UINT16_MAX),
                      (uint16_t)autk_uint32_min(params->height ? params->height : 100, UINT16_MAX),
                      0, XCB_WINDOW_CLASS_INPUT_OUTPUT, client_data->default_visual->visual_id,
                      XCB_CW_EVENT_MASK, value_list);

    AUTK_TRY(set_wm_protocols(client_data, window_data));

    // Register the window with the client's window map.
    AUTK_TRY(autk_x11_window_map_insert(&client_data->window_map, window_data->window_id, window));

    return AUTK_OK;
}

static void
autk_x11_window_fini(autk_window_t *window, void *opaque_driver_data)
{
    autk_x11_window_data_t *window_data = opaque_driver_data;
    autk_x11_client_data_t *client_data = window->client->driver_data;

    if (window_data->window_id != 0) {
        // Send a destroy request.
        xcb_destroy_window(window_data->connection, window_data->window_id);

        // Unregister the window from the client's window map.
        autk_x11_window_map_remove(&client_data->window_map, window_data->window_id);

        window_data->window_id = 0;
    }
}

static autk_status_t
autk_x11_window_set_visible(autk_window_t *window, void *opaque_driver_data, bool visible)
{
    autk_x11_window_data_t *window_data = opaque_driver_data;

    (void)window;

    if (window_data->window_id) {
        if (visible) {
            xcb_map_window(window_data->connection, window_data->window_id);
        } else {
            xcb_unmap_window(window_data->connection, window_data->window_id);
        }
    } else if (visible) {
        return AUTK_ERR_RESOURCE_LOST;
    }

    return AUTK_OK;
}

AUTK_HIDDEN const autk_window_driver_t autk_window_driver_x11 = {
    .struct_size = sizeof(autk_window_driver_t),
    .driver_data_size = sizeof(autk_x11_window_data_t),

    .init = autk_x11_window_init,
    .fini = autk_x11_window_fini,
    .set_visible = autk_x11_window_set_visible,
};

//==============================================================================
//
// X11 window map
//
//==============================================================================

static autk_hash_t
window_map_node_hash(const void *opaque)
{
    return *(const uint32_t *)opaque * 32939;
}

static bool
window_map_node_eq(const void *opaque0, const void *opaque1)
{
    return *(const uint32_t *)opaque0 == *(const uint32_t *)opaque1;
}

AUTK_HIDDEN void
autk_x11_window_map_init(autk_instance_t *instance, autk_x11_window_map_t *map)
{
    autk_hash_table_init(instance, &map->ht, sizeof(autk_x11_window_map_node_t),
                         window_map_node_hash, window_map_node_eq);
}

AUTK_HIDDEN void
autk_x11_window_map_fini(autk_x11_window_map_t *map)
{
    autk_hash_table_fini(&map->ht);
}

AUTK_HIDDEN autk_window_t *
autk_x11_window_map_get(autk_x11_window_map_t *map, uint32_t id)
{
    autk_hash_iter_t iter;
    autk_x11_window_map_node_t *node;

    if (autk_hash_table_find(&map->ht, &id, &iter)) {
        node = autk_hash_table_get(&map->ht, iter);
        return node->window;
    } else {
        return NULL;
    }
}

AUTK_HIDDEN autk_status_t
autk_x11_window_map_insert(autk_x11_window_map_t *map, uint32_t id, autk_window_t *window)
{
    autk_x11_window_map_node_t node = {id, window};
    autk_x11_window_map_node_t *internal_node;
    autk_hash_iter_t iter;
    bool inserted;

    AUTK_TRY(autk_hash_table_insert(&map->ht, &node, &iter, &inserted));

    if (!inserted) {
        // The specified ID already belongs to another window. Invalidate that window.
        AUTK_WARN(autk_window_get_instance(window),
                  "Window ID %" PRIu32 " already exists--invalidating the old window", id);
        internal_node = autk_hash_table_get(&map->ht, iter);
        autk_x11_window_invalidate(internal_node->window);
        AUTK_TRY(autk_hash_table_insert(&map->ht, &node, &iter, NULL));
    }

    return AUTK_OK;
}

AUTK_HIDDEN autk_window_t *
autk_x11_window_map_remove(autk_x11_window_map_t *map, uint32_t id)
{
    autk_x11_window_map_node_t *node;

    node = autk_hash_table_remove(&map->ht, &id);
    return node ? node->window : NULL;
}
