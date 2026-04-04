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

#include <stdlib.h>
#include <string.h>

#include <autk/client.h>
#include <autk/diagnostics.h>
#include <autk/instance.h>
#include <autk/style.h>
#include <core/types.h>
#include <utility/math.h>

#define ALLOC_FALLBACK_STYLE_INCREMENT 4

static const autk_client_driver_t *
choose_default_driver(const autk_instance_t *instance)
{
#ifdef _WIN32
    return &autk_client_driver_windows;
#endif

#if AUTK_CLIENT_WAYLAND && 0 // remove '&& 0' to enable Wayland driver when it's ready
    char *wayland_display = getenv("WAYLAND_DISPLAY");
    if (wayland_display && wayland_display[0]) {
        return &autk_client_driver_wayland;
    }
#endif

#if AUTK_CLIENT_X11
    return &autk_client_driver_x11;
#endif

    AUTK_ERROR(instance, "No default client driver available");
    return NULL;
}

AUTK_API autk_status_t
autk_client_create(autk_instance_t *instance, const autk_client_create_params_t *params,
                   autk_client_t **out_client)
{
    static const autk_client_create_params_t default_params = {
        .struct_size = sizeof(autk_client_create_params_t),
    };

    const autk_client_driver_t *driver;
    size_t alloc_size = autk_align_up(sizeof(autk_client_t));
    size_t driver_data_offset = 0;
    size_t device_offset = 0;
    size_t device_driver_data_offset = 0;
    size_t user_data_offset = 0;
    autk_client_t *client;
    autk_status_t status;

    if (!instance || !out_client) {
        return AUTK_ERR_INVALID_ARGUMENT;
    }

    // Use default parameters if unspecified, then validate.
    if (!params) {
        params = &default_params;
    }

    if (params->struct_size != sizeof(autk_client_create_params_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    } else if (params->app_style_count > 0 && !params->app_styles) {
        return AUTK_ERR_INVALID_ARGUMENT;
    }

    for (uint16_t i = 0; i < params->app_style_count; i++) {
        if (!params->app_styles[i]) {
            return AUTK_ERR_INVALID_ARGUMENT;
        }
    }

    // Choose a driver if unspecified, then validate.
    driver = params->driver ? params->driver : choose_default_driver(instance);
    if (!driver) {
        return AUTK_ERR_INVALID_CONFIGURATION;
    } else if (driver->struct_size != sizeof(autk_client_driver_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    // Compute the actual size of the client object.
    AUTK_TRY(autk_add_alloc_region(&alloc_size, driver->driver_data_size, &driver_data_offset));
    AUTK_TRY(autk_add_alloc_region(&alloc_size, sizeof(autk_device_t), &device_offset));
    AUTK_TRY(autk_add_alloc_region(
        &alloc_size, driver->device_driver ? driver->device_driver->driver_data_size : 0,
        &device_driver_data_offset));
    AUTK_TRY(autk_add_alloc_region(&alloc_size, params->user_data_size, &user_data_offset));

    // Allocate the client object.
    client = autk_instance_alloc(instance, NULL, 0, alloc_size, AUTK_MEMORY_TAG_CLIENT);
    if (!client) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }

    // Initialize the client object.
    *client = (autk_client_t){
        .driver = driver,
        .alloc_size = alloc_size,
        .instance = instance,
        .callbacks = params->callbacks,
        .driver_data = driver->driver_data_size ? (char *)client + driver_data_offset : NULL,
        .device = (autk_device_t *)((char *)client + device_offset),
        .user_data = params->user_data_size ? (char *)client + user_data_offset : NULL,
    };

    *client->device = (autk_device_t){
        .driver = driver->device_driver,
        .instance = instance,
        .driver_data = driver->device_driver ? (char *)client + device_driver_data_offset : NULL,
        .user_data = params->user_data_size ? (char *)client + user_data_offset : NULL,
    };

    if (driver->driver_data_size) {
        memset(client->driver_data, 0, driver->driver_data_size);
    }

    if (params->user_data_size) {
        if (params->user_data_init) {
            memcpy(client->user_data, params->user_data_init, params->user_data_size);
        } else {
            memset(client->user_data, 0, params->user_data_size);
        }
    }

    // Copy app styles if provided.
    if (params->app_style_count > 0) {
        client->app_style_capacity = params->app_style_count;
        client->app_style_count = params->app_style_count;
        client->app_styles = autk_instance_alloc(
            instance, NULL, 0, client->app_style_capacity * sizeof(autk_style_t *),
            AUTK_MEMORY_TAG_LIST);

        if (!client->app_styles) {
            status = AUTK_ERR_OUT_OF_MEMORY;
            goto err_free;
        }

        for (uint16_t i = 0; i < params->app_style_count; i++) {
            client->app_styles[i] = autk_style_retain(params->app_styles[i]);
        }
    }

    // Initialize the client driver.
    if (driver->init) {
        status = driver->init(client, client->driver_data, params);
        if (status != AUTK_OK) {
            goto err_fini_client_driver;
        }
    }

    // Initialize the device driver.
    if (client->device->driver && client->device->driver->init_from_client) {
        status = client->device->driver->init_from_client(
            client->device, client->device->driver_data, client, client->driver_data, params);

        if (status != AUTK_OK) {
            goto err_fini_device_driver;
        }
    }

    // Success!
    *out_client = client;
    return AUTK_OK;

    // Clean up on error.
err_fini_device_driver:
    if (client->device->driver && client->device->driver->fini) {
        client->device->driver->fini(client->device, client->device->driver_data);
    }
err_fini_client_driver:
    if (driver->fini) {
        driver->fini(client, client->driver_data);
    }
err_free:
    autk_instance_alloc(instance, client, alloc_size, 0, AUTK_MEMORY_TAG_CLIENT);
    return status;
}

AUTK_API void
autk_client_destroy(autk_client_t *client)
{
    if (!client) {
        return;
    }

    // Clean up the driver data.
    if (client->driver->fini) {
        client->driver->fini(client, client->driver_data);
    }

    // Release all cached styles.
    for (uint16_t i = 0; i < client->app_style_count; i++) {
        autk_style_release(client->app_styles[i]);
    }
    if (client->app_styles) {
        autk_instance_alloc(client->instance, client->app_styles,
                            client->app_style_capacity * sizeof(autk_style_t *), 0,
                            AUTK_MEMORY_TAG_LIST);
    }
    for (uint16_t i = 0; i < client->fallback_style_count; i++) {
        autk_style_release(client->fallback_styles[i]);
    }
    if (client->fallback_styles) {
        autk_instance_alloc(client->instance, client->fallback_styles,
                            client->fallback_style_capacity * sizeof(autk_style_t *), 0,
                            AUTK_MEMORY_TAG_LIST);
    }

    // Free the client object.
    autk_instance_alloc(client->instance, client, client->alloc_size, 0, AUTK_MEMORY_TAG_CLIENT);
}

AUTK_API autk_status_t
autk_client_run(autk_client_t *client)
{
    if (!client) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (!client->driver->run) {
        return AUTK_ERR_UNIMPLEMENTED;
    }

    return client->driver->run(client, client->driver_data);
}

AUTK_API autk_status_t
autk_client_quit(autk_client_t *client)
{
    if (!client) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (!client->driver->quit) {
        return AUTK_ERR_UNIMPLEMENTED;
    }

    return client->driver->quit(client, client->driver_data);
}

AUTK_API autk_device_t *
autk_client_get_device(autk_client_t *client)
{
    return client ? client->device : NULL;
}

AUTK_API void *
autk_client_get_driver_data(autk_client_t *client)
{
    return client ? client->driver_data : NULL;
}

AUTK_API autk_instance_t *
autk_client_get_instance(const autk_client_t *client)
{
    return client ? client->instance : NULL;
}

AUTK_API void *
autk_client_get_user_data(autk_client_t *client)
{
    return client ? client->user_data : NULL;
}

AUTK_API autk_status_t
autk_client_find_or_create_style_extension(autk_client_t *client,
                                           const autk_extension_query_t *query,
                                           autk_style_t **out_style,
                                           const autk_extension_header_t **out_ext,
                                           const autk_style_class_t *fallback_class)
{
    autk_style_t *style;
    const autk_extension_header_t *ext;
    autk_style_create_params_t create_params;
    void *alloc;

    if (!client || !query) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (fallback_class && fallback_class->struct_size != sizeof(autk_style_class_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    // Look for a matching app style.
    for (uint16_t i = 0; i < client->app_style_count; i++) {
        style = client->app_styles[i];
        ext = autk_style_find_extension(style, query);
        if (ext) {
            if (out_style) {
                *out_style = style;
            }
            if (out_ext) {
                *out_ext = ext;
            }
            return AUTK_OK;
        }
    }

    // Look for a matching fallback style.
    for (uint16_t i = 0; i < client->fallback_style_count; i++) {
        style = client->fallback_styles[i];
        ext = autk_style_find_extension(style, query);
        if (ext) {
            if (out_style) {
                *out_style = style;
            }
            if (out_ext) {
                *out_ext = ext;
            }
            return AUTK_OK;
        }
    }

    // No match found. Create a fallback if provided.
    if (fallback_class) {
        // Make sure the provided class provides the requested extension.
        if (!autk_style_class_provides_extension(fallback_class, query)) {
            return AUTK_ERR_UNSUPPORTED_EXTENSION;
        }

        // Allocate enough storage for another fallback style.
        if (client->fallback_style_capacity < client->fallback_style_count + 1) {
            alloc = autk_instance_alloc(
                client->instance, client->fallback_styles,
                client->fallback_style_capacity * sizeof(autk_style_t *),
                (client->fallback_style_capacity + ALLOC_FALLBACK_STYLE_INCREMENT)
                    * sizeof(autk_style_t *),
                AUTK_MEMORY_TAG_LIST);

            if (!alloc) {
                return AUTK_ERR_OUT_OF_MEMORY;
            }

            client->fallback_styles = alloc;
            client->fallback_style_capacity += ALLOC_FALLBACK_STYLE_INCREMENT;
        }

        // Create the fallback style.
        create_params = (autk_style_create_params_t){
            .struct_size = sizeof(autk_style_create_params_t),
            .klass = fallback_class,
        };

        AUTK_TRY(autk_style_create(client->instance, &create_params, &style));
        client->fallback_styles[client->fallback_style_count++] = style;

        if (out_style) {
            *out_style = style;
        }
        if (out_ext) {
            *out_ext = autk_style_find_extension(style, query);
        }
        return AUTK_OK;
    } else {
        return AUTK_ERR_UNSUPPORTED_EXTENSION;
    }
}
