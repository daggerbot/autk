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

#include <string.h>

#include <autk/client.h>
#include <autk/diagnostics.h>
#include <autk/ext/style_ext_base.h>
#include <autk/style.h>
#include <autk/window.h>
#include <core/types.h>
#include <utility/math.h>

static autk_status_t
init_background_color(autk_window_t *window, const autk_window_create_params_t *params)
{
    static const autk_extension_query_t query = {
        .uuid = AUTK_STYLE_EXTENSION_BASE_INIT,
        .min_version = 1,
    };
    autk_style_t *style;
    const autk_extension_header_t *ext;

    // Choose a background color from params or style defaults.
    if (params->flags & AUTK_WINDOW_CREATE_FLAG_BACKGROUND_COLOR) {
        window->background_color = params->background_color;
        window->flags |= AUTK_WINDOW_FLAG_EXPLICIT_BACKGROUND_COLOR;
    } else {
        AUTK_TRY(autk_client_find_or_create_style_extension(window->client, &query, &style, &ext,
                                                            autk_style_class_default));
        window->background_color =
            autk_style_ext_base_get_default_window_background_color(style, ext, params->type);
    }

    // Set the background color.
    if (window->driver->set_background_color) {
        AUTK_TRY(window->driver->set_background_color(window, window->driver_data,
                                                      window->background_color));
    }

    return AUTK_OK;
}

AUTK_API autk_status_t
autk_window_create(autk_client_t *client, const autk_window_create_params_t *params,
                   autk_window_t **out_window)
{
    static const autk_window_create_params_t default_params = {
        .struct_size = sizeof(autk_window_create_params_t),
    };

    const autk_window_driver_t *driver;
    size_t alloc_size = autk_align_up(sizeof(autk_window_t));
    size_t driver_data_offset = 0;
    size_t user_data_offset = 0;
    autk_window_t *window;
    autk_status_t status;

    if (!client || !out_window) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (!client->driver->window_driver) {
        return AUTK_ERR_UNIMPLEMENTED;
    } else if (client->driver->window_driver->struct_size != sizeof(autk_window_driver_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    driver = client->driver->window_driver;

    // Use default parameters if unspecified.
    if (!params) {
        params = &default_params;
    }

    // Validate parameters.
    if (params->struct_size != sizeof(autk_window_create_params_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    } else if (params->flags & (autk_window_create_flags_t)~AUTK_WINDOW_CREATE_FLAG_ALL) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (params->callbacks
               && params->callbacks->struct_size != sizeof(autk_window_callbacks_t))
    {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    // Compute the size and layout of the window object.
    AUTK_TRY(autk_add_alloc_region(&alloc_size, driver->driver_data_size, &driver_data_offset));
    AUTK_TRY(autk_add_alloc_region(&alloc_size, params->user_data_size, &user_data_offset));

    // Allocate and initialize the window object.
    window = autk_instance_alloc(client->instance, NULL, 0, alloc_size, AUTK_MEMORY_TAG_WINDOW);
    if (!window) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }

    *window = (autk_window_t){
        .driver = driver,
        .alloc_size = alloc_size,
        .instance = client->instance,
        .client = client,
        .callbacks = params->callbacks,
        .driver_data = driver->driver_data_size ? (char *)window + driver_data_offset : NULL,
        .user_data = params->user_data_size ? (char *)window + user_data_offset : NULL,
    };

    // Initialize the driver data.
    if (driver->driver_data_size) {
        memset(window->driver_data, 0, driver->driver_data_size);
    }
    if (driver->init) {
        status = driver->init(window, window->driver_data, params);
        if (status != AUTK_OK) {
            if (driver->fini) {
                driver->fini(window, window->driver_data);
            }
            autk_instance_alloc(client->instance, window, alloc_size, 0, AUTK_MEMORY_TAG_WINDOW);
            return status;
        }
    }

    // Set post-creation properties.
    AUTK_TRY(init_background_color(window, params));

    // Initialize the user data region.
    if (params->user_data_size) {
        if (params->user_data_init) {
            memcpy(window->user_data, params->user_data_init, params->user_data_size);
        } else {
            memset(window->user_data, 0, params->user_data_size);
        }
    }

    // Success!
    *out_window = window;
    return AUTK_OK;
}

AUTK_API void
autk_window_destroy(autk_window_t *window)
{
    if (!window) {
        return;
    }

    // Notify the user finalizer.
    if (window->callbacks && window->callbacks->destroying) {
        window->callbacks->destroying(window, window->user_data);
    }

    // Notify the driver finalizer.
    if (window->driver->fini) {
        window->driver->fini(window, window->driver_data);
    }

    // Free the window object.
    autk_instance_alloc(window->instance, window, window->alloc_size, 0, AUTK_MEMORY_TAG_WINDOW);
}

AUTK_API autk_instance_t *
autk_window_get_instance(const autk_window_t *window)
{
    return window ? window->instance : NULL;
}

AUTK_API autk_client_t *
autk_window_get_client(const autk_window_t *window)
{
    return window ? window->client : NULL;
}

AUTK_API const autk_window_driver_t *
autk_window_get_driver(autk_window_t *window)
{
    return window ? window->driver : NULL;
}

AUTK_API void *
autk_window_get_driver_data(autk_window_t *window)
{
    return window ? window->driver_data : NULL;
}

AUTK_API void *
autk_window_get_user_data(autk_window_t *window)
{
    return window ? window->user_data : NULL;
}

AUTK_API autk_status_t
autk_window_set_title(autk_window_t *window, const char *title)
{
    if (!window) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (!window->driver->set_title) {
        return AUTK_ERR_UNIMPLEMENTED;
    }

    return window->driver->set_title(window, window->driver_data, title);
}

AUTK_API autk_status_t
autk_window_set_visible(autk_window_t *window, bool visible)
{
    if (!window) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (!window->driver->set_visible) {
        return AUTK_ERR_UNIMPLEMENTED;
    }

    return window->driver->set_visible(window, window->driver_data, visible);
}

AUTK_API void
autk_window_callback_destroy(autk_window_t *window, void *unused)
{
    (void)unused;
    autk_window_destroy(window);
}

AUTK_API void
autk_window_callback_quit(autk_window_t *window, void *unused)
{
    (void)unused;
    autk_client_quit(window->client);
}
