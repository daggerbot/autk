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

#ifndef AUTK_CORE_TYPES_H_
#define AUTK_CORE_TYPES_H_

#include <stdatomic.h>

#include <autk/types.h>

enum autk_window_flags {
    AUTK_WINDOW_FLAG_EXPLICIT_BACKGROUND_COLOR = 1 << 0,
};
typedef uint32_t autk_window_flags_t;

struct autk_client {
    const autk_client_driver_t *driver;
    size_t alloc_size;
    autk_instance_t *instance;

    // App-specified styles (e.g. autk_client_create_params_t)
    uint16_t app_style_capacity;
    uint16_t app_style_count;
    autk_style_t **app_styles;

    // Fallback styles (created lazily by widgets if no matching style extension is found)
    uint16_t fallback_style_capacity;
    uint16_t fallback_style_count;
    autk_style_t **fallback_styles;

    void *driver_data;
    autk_device_t *device;
    void *user_data;
};

struct autk_device {
    // Currently, a device is always owned by a client and part of its heap block. If/when we extend
    // the API to allow creating devices independently of clients, we'll need to add the usual
    // boilerplate.
    const autk_device_driver_t *driver;
    autk_instance_t *instance;
    void *driver_data;
    void *user_data;
};

struct autk_instance {
    size_t alloc_size;
    autk_instance_create_flags_t flags;
    autk_alloc_func_t alloc_func;
    void *alloc_ctx;
    autk_message_func_t message_func;
    void *message_ctx;
    void *user_data;
};

struct autk_style {
    const autk_style_class_t *klass;
    _Atomic uint32_t ref_count;
    size_t alloc_size;
    autk_instance_t *instance;
    void *class_data;
    void *user_data;
};

struct autk_window {
    const autk_window_driver_t *driver;
    size_t alloc_size;
    autk_instance_t *instance;
    autk_client_t *client;
    autk_window_flags_t flags;
    autk_rgba_t background_color;
    const autk_window_callbacks_t *callbacks;
    void *driver_data;
    void *user_data;
};

#endif // AUTK_CORE_TYPES_H_
