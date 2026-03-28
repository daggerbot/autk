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

#ifndef AUTK_CLIENT_H_
#define AUTK_CLIENT_H_

#include "types.h"

struct autk_window_driver;

typedef struct autk_client_create_params autk_client_create_params_t;
typedef struct autk_client_driver autk_client_driver_t;

struct autk_client_create_params {
    /// Size of this struct. Must be `sizeof(autk_client_create_params_t)`.
    size_t struct_size;
    /// Pointer to the client driver to use, or `NULL` to use the default driver.
    const autk_client_driver_t *driver;
    /// Display name used by the X11 and Wayland drivers when opening a connection to the display
    /// server.
    const char *display_name;
    /// Pointer to additional data that a custom driver may use when initializing the client. All
    /// built-in drivers ignore this.
    void *driver_init_ctx;
    /// Number of bytes to allocate for application use.
    size_t user_data_size;
    /// Pointer to the initial value of the user data region. If `NULL`, the user data is zeroed.
    const void *user_data_init;
};

struct autk_client_driver {
    /// Size of this struct. Must be `sizeof(autk_client_driver_t)`.
    size_t struct_size;
    /// Number of bytes to allocate for driver use.
    size_t driver_data_size;

    const struct autk_window_driver *window_driver;

    /// Function called to initialize the client's driver data after is it allocated.
    autk_status_t (*init)(autk_client_t *client, void *driver_data,
                          const autk_client_create_params_t *params);
    /// Function called to clean up the client's driver data.
    void (*fini)(autk_client_t *client, void *driver_data);
    autk_status_t (*run)(autk_client_t *client, void *driver_data);
    autk_status_t (*push_job)(autk_client_t *client, void *driver_data, autk_job_t job, bool block,
                              bool *queued);
    autk_status_t (*quit)(autk_client_t *client, void *driver_data);

    // more later
};

AUTK_API extern const autk_client_driver_t autk_client_driver_x11;
AUTK_API extern const autk_client_driver_t autk_client_driver_wayland;
AUTK_API extern const autk_client_driver_t autk_client_driver_windows;
AUTK_API extern const autk_client_driver_t autk_client_driver_macos;

AUTK_BEGIN_DECLS

AUTK_API autk_status_t
autk_client_create(autk_instance_t *instance, const autk_client_create_params_t *params,
                   autk_client_t **out_client);

AUTK_API void
autk_client_destroy(autk_client_t *client);

AUTK_API autk_status_t
autk_client_run(autk_client_t *client);

AUTK_API autk_status_t
autk_client_try_push_job(autk_client_t *client, autk_job_t job, bool *queued);

AUTK_API autk_status_t
autk_client_quit(autk_client_t *client);

AUTK_API autk_instance_t *
autk_client_get_instance(const autk_client_t *client);

AUTK_API void *
autk_client_get_driver_data(autk_client_t *client);

AUTK_API void *
autk_client_get_user_data(autk_client_t *client);

AUTK_END_DECLS

#endif // AUTK_CLIENT_H_
