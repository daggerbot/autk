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

#ifndef AUTK_INSTANCE_H_
#define AUTK_INSTANCE_H_

#include "types.h"

enum autk_instance_flags {
    /// Indicates that the provided allocator is thread-safe.
    /// If not, some features may be unavailable.
    AUTK_INSTANCE_FLAGS_THREAD_SAFE_ALLOC = 1 << 0,
    /// Indicates that the provided message hook is thread-safe.
    /// If not, some messages may be skipped.
    AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE = 1 << 1,

    AUTK_INSTANCE_FLAGS_ALL =
        AUTK_INSTANCE_FLAGS_THREAD_SAFE_ALLOC | AUTK_INSTANCE_FLAGS_THREAD_SAFE_MESSAGE,
    AUTK_INSTANCE_32BIT_ = 0x7FFFFFFFul,
};
/// \see \ref autk_instance_flags
typedef uint32_t autk_instance_flags_t;

typedef void *(*autk_alloc_func_t)(void *ctx, void *mem, size_t old_size, size_t new_size,
                                   autk_memory_tag_t tag);

typedef void (*autk_message_func_t)(void *ctx, autk_message_severity_t severity,
                                    const char *module_name, const autk_source_location_t *location,
                                    const char *message);

typedef struct autk_lifetime_hooks {
    void (*created)(void *ctx, autk_instance_t *instance, void *child,
                    autk_object_type_t child_type, void *parent, autk_object_type_t parent_type);
    void (*destroying)(void *ctx, autk_instance_t *instance, void *child,
                       autk_object_type_t child_type, void *parent, autk_object_type_t parent_type);
    void (*reparenting)(void *ctx, autk_instance_t *instance, void *child,
                        autk_object_type_t child_type, void *old_parent,
                        autk_object_type_t old_parent_type, void *new_parent,
                        autk_object_type_t new_parent_type);
} autk_lifetime_hooks_t;

typedef struct autk_instance_create_params {
    size_t struct_size;
    autk_instance_flags_t flags;
    autk_alloc_func_t alloc_func;
    void *alloc_ctx;
    autk_message_func_t message_func;
    void *message_ctx;
    const autk_lifetime_hooks_t *lifetime_hooks;
    void *lifetime_hooks_ctx;
    size_t user_data_size;
    const void *user_data_init;
} autk_instance_create_params_t;

AUTK_BEGIN_DECLS

AUTK_API autk_status_t
autk_instance_create(const autk_instance_create_params_t *params, autk_instance_t **out_instance);

AUTK_API void
autk_instance_destroy(autk_instance_t *instance);

AUTK_API void *
autk_instance_get_user_data(autk_instance_t *instance);

AUTK_API void
autk_instance_message(const autk_instance_t *instance, autk_message_severity_t severity,
                      const char *module_name, const autk_source_location_t *location,
                      const char *message);

AUTK_API bool
autk_instance_message_f(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, ...) AUTK_FMT(5, 6);

AUTK_API bool
autk_instance_message_v(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, va_list args) AUTK_VFMT(5, 6);

AUTK_API void *
autk_instance_alloc(const autk_instance_t *instance, void *mem, size_t old_size, size_t new_size,
                    autk_memory_tag_t tag);

AUTK_API void *
autk_default_alloc(void *unused, void *mem, size_t old_size, size_t new_size,
                   autk_memory_tag_t tag);

AUTK_END_DECLS

#endif // AUTK_INSTANCE_H_
