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

AUTK_API autk_device_t *
autk_client_get_device(autk_client_t *client);

AUTK_API void *
autk_client_get_driver_data(autk_client_t *client);

AUTK_API autk_instance_t *
autk_client_get_instance(const autk_client_t *client);

AUTK_API void *
autk_client_get_user_data(autk_client_t *client);

AUTK_API autk_status_t
autk_client_find_or_create_style_extension(autk_client_t *client,
                                           const autk_extension_query_t *query,
                                           autk_style_t **out_style,
                                           const autk_extension_header_t **out_ext,
                                           const autk_style_class_t *fallback_class);

AUTK_END_DECLS

#endif // AUTK_CLIENT_H_
