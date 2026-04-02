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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <autk/diagnostics.h>
#include <autk/instance.h>
#include <core/types.h>
#include <utility/math.h>

AUTK_API autk_status_t
autk_instance_create(const autk_instance_create_params_t *params, autk_instance_t **out_instance)
{
    static const autk_instance_create_params_t default_params = {
        .struct_size = sizeof(autk_instance_create_params_t),
    };

    autk_instance_create_flags_t flags;
    autk_alloc_func_t alloc_func;
    autk_instance_t *instance;
    size_t alloc_size;
    size_t user_data_offset;
    size_t user_data_size = 0;

    if (!out_instance) {
        return AUTK_ERR_INVALID_ARGUMENT;
    }

    // Use default parameters if unspecified.
    if (!params) {
        params = &default_params;
    }

    // Validate parameters.
    if (params->struct_size != sizeof(autk_instance_create_params_t)) {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    flags = params->flags;

    if (params->alloc_func) {
        alloc_func = params->alloc_func;
    } else {
        alloc_func = &autk_default_alloc;
        flags |= AUTK_INSTANCE_CREATE_FLAG_THREAD_SAFE_ALLOC;
    }

    if (params->flags & (autk_instance_create_flags_t)~AUTK_INSTANCE_CREATE_FLAG_ALL) {
        return AUTK_ERR_INVALID_ARGUMENT;
    }

    // Compute the actual size of the instance object.
    alloc_size = autk_align_up(sizeof(autk_instance_t));
    user_data_offset = alloc_size;

    if (params->user_data_size > 0) {
        user_data_size = autk_align_up(params->user_data_size);
        if (!user_data_size || user_data_size > SIZE_MAX - alloc_size) {
            return AUTK_ERR_ARITHMETIC_OVERFLOW;
        }
        alloc_size += user_data_size;
    }

    // Allocate and initialize the instance.
    instance = alloc_func(params->alloc_ctx, NULL, 0, alloc_size, AUTK_MEMORY_TAG_INSTANCE);
    if (!instance) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }

    *instance = (autk_instance_t){
        .alloc_size = alloc_size,
        .flags = flags,
        .alloc_func = alloc_func,
        .alloc_ctx = params->alloc_ctx,
        .message_func = params->message_func,
        .message_ctx = params->message_ctx,
        .user_data = params->user_data_size ? (char *)instance + user_data_offset : NULL,
    };

    // Initialize user data.
    if (user_data_size) {
        if (params->user_data_init) {
            memcpy(instance->user_data, params->user_data_init, params->user_data_size);
        } else {
            memset(instance->user_data, 0, user_data_size);
        }
    }

    // Success!
    *out_instance = instance;
    return AUTK_OK;
}

AUTK_API void
autk_instance_destroy(autk_instance_t *instance)
{
    if (!instance) {
        return;
    }

    autk_instance_alloc(instance, instance, instance->alloc_size, 0, AUTK_MEMORY_TAG_INSTANCE);
}

AUTK_API void *
autk_instance_get_user_data(autk_instance_t *instance)
{
    return instance ? instance->user_data : NULL;
}

AUTK_API void
autk_instance_message(const autk_instance_t *instance, autk_message_severity_t severity,
                      const char *module_name, const autk_source_location_t *location,
                      const char *message)
{
    if (!instance || !instance->message_func) {
        return;
    }

    instance->message_func(instance->message_ctx, severity, module_name, location, message);
}

AUTK_API bool
autk_instance_message_f(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, ...)
{
    va_list args;
    bool result;

    va_start(args, fmt);
    result = autk_instance_message_v(instance, severity, module_name, location, fmt, args);
    va_end(args);
    return result;
}

AUTK_API bool
autk_instance_message_v(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, va_list args)
{
    char stack_buf[512];
    va_list args_copy;
    int result;
    size_t buf_size;
    char *heap_buf;

    if (!instance || !instance->message_func) {
        return true;
    }

    /* We may end up calling vsnprintf() twice. */
    va_copy(args_copy, args);

    /* Try formatting to the stack buffer first. */
    result = vsnprintf(stack_buf, sizeof(stack_buf), fmt, args);
    if (result < 0) {
        /* Formatting failed. */
        va_end(args_copy);
        return false;
    } else if ((size_t)result < sizeof(stack_buf)) {
        /* Formatting succeeded without truncation. */
        autk_instance_message(instance, severity, module_name, location, stack_buf);
        va_end(args_copy);
        return true;
    }

    /* The stack buffer is too small. Fall back to a heap buffer. */
    buf_size = (size_t)result + 1;
    heap_buf = autk_instance_alloc(instance, NULL, 0, buf_size, AUTK_MEMORY_TAG_STRING);
    if (!heap_buf) {
        /* Out of memory. Reporting a partial message is better than nothing. */
        autk_instance_message(instance, severity, module_name, location, stack_buf);
        va_end(args_copy);
        return true;
    }

    result = vsnprintf(heap_buf, buf_size, fmt, args_copy);
    va_end(args_copy);
    if (result < 0) {
        /* Formatting failed. Reporting a partial message is better than nothing. */
        autk_instance_message(instance, severity, module_name, location, stack_buf);
    } else {
        autk_instance_message(instance, severity, module_name, location, heap_buf);
    }

    autk_instance_alloc(instance, heap_buf, buf_size, 0, AUTK_MEMORY_TAG_STRING);
    return true;
}

AUTK_API void *
autk_instance_alloc(const autk_instance_t *instance, void *mem, size_t old_size, size_t new_size,
                    autk_memory_tag_t tag)
{
    if (old_size == new_size) {
        return mem;
    } else if (!instance) {
        return NULL;
    }

    return instance->alloc_func(instance->alloc_ctx, mem, old_size, new_size, tag);
}

AUTK_API void *
autk_default_alloc(void *unused, void *mem, size_t old_size, size_t new_size, autk_memory_tag_t tag)
{
    (void)unused;
    (void)old_size;
    (void)tag;

    if (new_size == 0) {
        if (mem) {
            free(mem);
        }
        return NULL;
    }

    if (mem) {
        return realloc(mem, new_size);
    } else {
        return malloc(new_size);
    }
}
