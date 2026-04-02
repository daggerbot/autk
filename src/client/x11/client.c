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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>

#include <autk/diagnostics.h>

#include "client.h"
#include "window.h"

//==============================================================================
//
// X11 client driver
//
//==============================================================================

static autk_status_t
check_connection(autk_instance_t *instance, autk_x11_client_data_t *client_data)
{
    int error_code;

    error_code = xcb_connection_has_error(client_data->connection);

    switch (error_code) {
        case 0:
            return AUTK_OK;
        case XCB_CONN_ERROR:
            AUTK_ERROR(instance, "XCB connection error");
            return AUTK_ERR_IO_FAILURE;
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
            AUTK_ERROR(instance, "Unsupported X11 extension");
            return AUTK_ERR_UNSUPPORTED_EXTENSION;
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
            AUTK_ERROR(instance, "Out of memory");
            return AUTK_ERR_OUT_OF_MEMORY;
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
            AUTK_ERROR(instance, "X11 request length exceeded");
            return AUTK_ERR_PROTOCOL_VIOLATION;
        case XCB_CONN_CLOSED_PARSE_ERR:
            AUTK_ERROR(instance, "Failed to parse X11 display name");
            return AUTK_ERR_INVALID_CONFIGURATION;
        case XCB_CONN_CLOSED_INVALID_SCREEN:
            AUTK_ERROR(instance, "Invalid X11 screen index");
            return AUTK_ERR_INVALID_CONFIGURATION;
        case XCB_CONN_CLOSED_FDPASSING_FAILED:
            AUTK_ERROR(instance, "File descriptor passing failed");
            return AUTK_ERR_IO_FAILURE;
        default:
            AUTK_ERROR(instance, "XCB connection error code %d", error_code);
            return AUTK_ERR_IO_FAILURE;
    }
}

static autk_status_t
find_default_screen(autk_instance_t *instance, autk_x11_client_data_t *client_data)
{
    const xcb_setup_t *setup;
    int screen_index = 0;
    xcb_screen_iterator_t screen_iter;

    setup = xcb_get_setup(client_data->connection);

    // Find the default screen struct.
    screen_iter = xcb_setup_roots_iterator(setup);
    for (; screen_iter.rem > 0; xcb_screen_next(&screen_iter)) {
        if (screen_index == client_data->default_screen_num) {
            client_data->default_screen = screen_iter.data;
            break;
        }
        screen_index++;
    }

    if (!client_data->default_screen) {
        AUTK_ERROR(instance, "Can't find default X11 screen: index=%d max=%d",
                   client_data->default_screen_num, xcb_setup_roots_length(setup));
        return AUTK_ERR_INVALID_CONFIGURATION;
    }

    return AUTK_OK;
}

static bool
get_color_shift_component(uint32_t mask, uint8_t *shift)
{
    *shift = 0;
    while (mask && !(mask & 1)) {
        mask >>= 1;
        (*shift)++;
    }
    return mask == 0xFF;
}

static bool
get_color_shift(const xcb_visualtype_t *visual, autk_rgb_t *color_shift)
{
    return get_color_shift_component(visual->red_mask, &color_shift->r)
           && get_color_shift_component(visual->green_mask, &color_shift->g)
           && get_color_shift_component(visual->blue_mask, &color_shift->b);
}

static autk_status_t
choose_default_visual(autk_instance_t *instance, autk_x11_client_data_t *client_data)
{
    xcb_depth_iterator_t depth_iter;
    uint8_t depth;
    xcb_visualtype_iterator_t visual_iter;
    xcb_visualtype_t *visual;
    xcb_visualtype_t *best_24bit_visual = NULL;
    xcb_visualtype_t *best_32bit_visual = NULL;
    autk_rgb_t color_shift;
    autk_rgb_t best_24bit_color_shift;
    autk_rgb_t best_32bit_color_shift;

    depth_iter = xcb_screen_allowed_depths_iterator(client_data->default_screen);
    for (; depth_iter.rem > 0; xcb_depth_next(&depth_iter)) {
        depth = depth_iter.data->depth;
        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem > 0; xcb_visualtype_next(&visual_iter)) {
            visual = visual_iter.data;

            // Check if this visual is suitable.
            if ((depth == 24 || depth == 32) && visual->_class == XCB_VISUAL_CLASS_TRUE_COLOR
                && visual->bits_per_rgb_value == 8 && get_color_shift(visual, &color_shift))
            {
                if (visual->visual_id == client_data->default_screen->root_visual) {
                    // The root visual takes priority over any other visual.
                    client_data->default_visual = visual;
                    client_data->default_depth = depth;
                    client_data->color_shift = color_shift;
                    return AUTK_OK;
                } else if (depth == 24 && !best_24bit_visual) {
                    best_24bit_visual = visual;
                    best_24bit_color_shift = color_shift;
                } else if (depth == 32 && !best_32bit_visual) {
                    best_32bit_visual = visual;
                    best_32bit_color_shift = color_shift;
                }
            }
        }
    }

    if (best_24bit_visual) {
        client_data->default_visual = best_24bit_visual;
        client_data->default_depth = 24;
        client_data->color_shift = best_24bit_color_shift;
    } else if (best_32bit_visual) {
        client_data->default_visual = best_32bit_visual;
        client_data->default_depth = 32;
        client_data->color_shift = best_32bit_color_shift;
    } else {
        AUTK_ERROR(instance, "No suitable X11 visual found");
        return AUTK_ERR_INVALID_CONFIGURATION;
    }

    return AUTK_OK;
}

static autk_status_t
intern_atoms(autk_instance_t *instance, autk_x11_client_data_t *client_data)
{
    static const struct {
        const char *name;
        size_t offset;
    } atom_defs[] = {
#define AUTK_DO(name) {#name, offsetof(autk_x11_atoms_t, atom_##name)},
        AUTK_FOR_EACH_X11_ATOM(AUTK_DO)
#undef AUTK_DO
    };

    xcb_intern_atom_cookie_t cookies[sizeof(atom_defs) / sizeof(atom_defs[0])];
    xcb_intern_atom_reply_t *reply;

    // Send all intern atom requests in one batch, rather than round-tripping for each one.
    for (size_t i = 0; i < sizeof(atom_defs) / sizeof(atom_defs[0]); i++) {
        cookies[i] = xcb_intern_atom(client_data->connection, 0,
                                     (uint16_t)strlen(atom_defs[i].name), atom_defs[i].name);
    }

    // Await all replies and store the atom values.
    xcb_flush(client_data->connection);
    for (size_t i = 0; i < sizeof(atom_defs) / sizeof(atom_defs[0]); i++) {
        reply = xcb_intern_atom_reply(client_data->connection, cookies[i], NULL);
        if (!reply) {
            AUTK_ERROR(instance, "Failed to intern X11 atom: %s", atom_defs[i].name);
            return AUTK_ERR_REQUEST_DENIED;
        }
        *(uint32_t *)((char *)&client_data->atoms + atom_defs[i].offset) = reply->atom;
        free(reply);
    }

    return AUTK_OK;
}

static void
create_default_colormap(autk_x11_client_data_t *client_data)
{
    uint32_t colormap;

    colormap = xcb_generate_id(client_data->connection);
    xcb_create_colormap(client_data->connection, XCB_COLORMAP_ALLOC_NONE, colormap,
                        client_data->default_screen->root, client_data->default_visual->visual_id);
    client_data->default_colormap = colormap;
}

static autk_status_t
autk_x11_client_init(autk_client_t *client, void *opaque_client_data,
                     const autk_client_create_params_t *params)
{
    autk_x11_client_data_t *client_data = opaque_client_data;
    autk_status_t status;

    // Connect to the X11 server.
    client_data->connection = xcb_connect(params->display_name, &client_data->default_screen_num);
    status = check_connection(client->instance, client_data);
    if (status != AUTK_OK) {
        client_data->connection = NULL; // Don't free dummy error connections.
        return status;
    }
    client_data->display_fd = xcb_get_file_descriptor(client_data->connection);

    // Initialize other client resources.
    AUTK_TRY(find_default_screen(client->instance, client_data));
    AUTK_TRY(choose_default_visual(client->instance, client_data));
    create_default_colormap(client_data);
    AUTK_TRY(intern_atoms(client->instance, client_data));
    autk_x11_window_map_init(client->instance, &client_data->window_map);
    AUTK_TRY(autk_posix_job_queue_init(&client_data->job_queue));

    return AUTK_OK;
}

static void
autk_x11_client_fini(autk_client_t *client, void *opaque_client_data)
{
    autk_x11_client_data_t *client_data = opaque_client_data;

    (void)client;

    autk_posix_job_queue_fini(&client_data->job_queue);
    autk_x11_window_map_fini(&client_data->window_map);

    if (client_data->default_colormap) {
        xcb_free_colormap(client_data->connection, client_data->default_colormap);
    }
    if (client_data->connection) {
        xcb_disconnect(client_data->connection);
    }
}

static autk_status_t
handle_wm_protocol(autk_x11_client_data_t *client_data, const xcb_client_message_event_t *event)
{
    uint32_t protocol = event->data.data32[0];
    autk_window_t *window;

    if (protocol == client_data->atoms.atom_WM_DELETE_WINDOW) {
        window = autk_x11_window_map_get(&client_data->window_map, event->window);
        if (window && window->callbacks && window->callbacks->close_requested) {
            window->callbacks->close_requested(window, window->user_data);
        }
    }

    return AUTK_OK;
}

static autk_status_t
handle_client_message(autk_x11_client_data_t *client_data, const xcb_client_message_event_t *event)
{
    if (event->type == client_data->atoms.atom_WM_PROTOCOLS && event->format == 32) {
        return handle_wm_protocol(client_data, event);
    }

    return AUTK_OK;
}

static autk_status_t
handle_xcb_error(autk_client_t *client, autk_x11_client_data_t *client_data,
                 const xcb_generic_error_t *error)
{
    autk_window_t *window;

    (void)client_data;

    // Report the raw error data.
    AUTK_ERROR(client->instance, "X11 error: code=%u rid=%" PRIu32 " major=%u minor=%u seq=%u",
               error->error_code, error->resource_id, error->major_code, error->minor_code,
               error->sequence);

    // If the error is failed window creation, invalidate the window so we don't keep making
    // requests with an invalid window ID.
    if (error->major_code == XCB_CREATE_WINDOW) {
        window = autk_x11_window_map_get(&client_data->window_map, error->resource_id);
        if (window) {
            AUTK_ERROR(client->instance, "X11 window creation failed for window id=%" PRIu32,
                       error->resource_id);
            autk_x11_window_invalidate(window->driver_data);
        }
    }

    return AUTK_OK;
}

static autk_status_t
handle_xcb_event(autk_client_t *client, autk_x11_client_data_t *client_data,
                 const xcb_generic_event_t *event)
{
    autk_window_t *window;

    switch (event->response_type & ~0x80) {
        case 0:
            return handle_xcb_error(client, client_data, (const xcb_generic_error_t *)event);

        case XCB_CLIENT_MESSAGE:
            return handle_client_message(client_data, (const xcb_client_message_event_t *)event);

        case XCB_DESTROY_NOTIFY:
            window = autk_x11_window_map_get(&client_data->window_map,
                                             ((xcb_destroy_notify_event_t *)event)->window);
            if (window) {
                autk_x11_window_invalidate(window->driver_data);
            }
            return AUTK_OK;

        default:
            return AUTK_OK;
    }
}

static autk_status_t
autk_x11_client_run(autk_client_t *client, void *opaque_client_data)
{
    autk_x11_client_data_t *client_data = opaque_client_data;
    autk_status_t status;
    autk_job_t job;
    int queue_result;
    int display_result;
    xcb_generic_event_t *event;

    client_data->quit_requested = false;

    while (!client_data->quit_requested) {
        // Execute all pending jobs before doing anything else.
        status = autk_posix_job_queue_try_pop(&client_data->job_queue, &job, client->instance);
        switch (status) {
            case AUTK_OK:
                if (job.exec) {
                    job.exec(job.ctx, client);
                }
                if (job.fini) {
                    job.fini(job.ctx);
                }
                continue;
            case AUTK_ERR_QUEUE_EMPTY:
            case AUTK_ERR_TRY_AGAIN:
                break;
            default:
                return status;
        }

        // Flush all pending X11 requests and check the connection.
        xcb_flush(client_data->connection);
        status = check_connection(client->instance, client_data);
        if (status != AUTK_OK) {
            return status;
        }

        // Block until any input is available.
        status = autk_posix_job_queue_poll(&client_data->job_queue, client_data->display_fd, -1,
                                           &queue_result, &display_result);
        switch (status) {
            case AUTK_OK:
                break;
            case AUTK_ERR_INTERRUPTED:
                continue;
            default:
                return status;
        }

        if (queue_result == -1 || display_result == -1) {
            return AUTK_ERR_IO_FAILURE;
        }

        // Check for any pending display events.
        if (display_result == 1) {
            while ((event = xcb_poll_for_event(client_data->connection)) != NULL) {
                status = handle_xcb_event(client, client_data, event);
                free(event);
                if (status != AUTK_OK) {
                    return status;
                }
                if (client_data->quit_requested) {
                    break;
                }
            }
        }
    }

    return AUTK_OK;
}

static autk_status_t
autk_x11_client_quit(autk_client_t *client, void *opaque_client_data)
{
    autk_x11_client_data_t *client_data = opaque_client_data;

    (void)client;

    client_data->quit_requested = true;
    return AUTK_OK;
}

AUTK_API const autk_client_driver_t autk_client_driver_x11 = {
    .struct_size = sizeof(autk_client_driver_t),
    .driver_data_size = sizeof(autk_x11_client_data_t),

    .window_driver = &autk_window_driver_x11,

    .init = autk_x11_client_init,
    .fini = autk_x11_client_fini,
    .run = autk_x11_client_run,
    .quit = autk_x11_client_quit,
};

//==============================================================================
//
// Internal API
//
//==============================================================================

AUTK_HIDDEN uint32_t
autk_x11_client_map_color(const autk_x11_client_data_t *client_data, autk_rgb_t color)
{
    return (((uint32_t)color.r << client_data->color_shift.r)
            | ((uint32_t)color.g << client_data->color_shift.g)
            | ((uint32_t)color.b << client_data->color_shift.b));
}
