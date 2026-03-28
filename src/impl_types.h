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

#ifndef AUTK_IMPL_TYPES_H_
#define AUTK_IMPL_TYPES_H_

#include <autk/client.h>
#include <autk/instance.h>
#include <autk/window.h>

struct autk_client {
    const autk_client_driver_t *driver;
    size_t alloc_size;
    autk_instance_t *instance;
    void *driver_data;
    void *user_data;
};

struct autk_instance {
    size_t alloc_size;
    autk_instance_flags_t flags;
    autk_alloc_func_t alloc_func;
    void *alloc_ctx;
    autk_message_func_t message_func;
    void *message_ctx;
    const autk_lifetime_hooks_t *lifetime_hooks;
    void *lifetime_hooks_ctx;
    void *user_data;
};

struct autk_window {
    const autk_window_driver_t *driver;
    size_t alloc_size;
    autk_instance_t *instance;
    autk_client_t *client;
    const autk_window_callbacks_t *callbacks;
    void *driver_data;
    void *user_data;
};

#endif // AUTK_IMPL_TYPES_H_
