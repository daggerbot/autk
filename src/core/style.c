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

#include <stdatomic.h>
#include <string.h>

#include <autk/ext/win9x_style.h>
#include <autk/instance.h>
#include <autk/math.h>
#include <autk/style.h>
#include <utility/math.h>

#include "types.h"

AUTK_API const autk_style_class_t *const autk_style_class_default = &autk_style_class_win9x;

AUTK_API autk_status_t
autk_style_create(autk_instance_t *instance, const autk_style_create_params_t *params,
                  autk_style_t **out_style)
{
    autk_style_t *style;
    autk_status_t status;
    size_t alloc_size = autk_align_up(sizeof(autk_style_t));
    size_t class_data_offset;
    size_t class_data_size = 0;
    size_t user_data_offset;
    size_t user_data_size = 0;

    if (!instance || !params || !params->klass || !params->klass->extension_count || !out_style) {
        return AUTK_ERR_INVALID_ARGUMENT;
    } else if (params->struct_size != sizeof(autk_style_create_params_t)
               || params->klass->struct_size != sizeof(autk_style_class_t))
    {
        return AUTK_ERR_INVALID_STRUCT_SIZE;
    }

    // Check for invalid or duplicate extensions.
    for (size_t i = 0; i < params->klass->extension_count; i++) {
        if (!params->klass->extensions[i]) {
            return AUTK_ERR_INVALID_ARGUMENT;
        } else if (params->klass->extensions[i]->struct_size < sizeof(autk_extension_header_t)) {
            return AUTK_ERR_INVALID_STRUCT_SIZE;
        } else if (autk_uuid_is_zero(&params->klass->extensions[i]->uuid)) {
            return AUTK_ERR_INVALID_ARGUMENT;
        }

        for (size_t j = i + 1; j < params->klass->extension_count; j++) {
            if (autk_uuid_equals(&params->klass->extensions[i]->uuid,
                                 &params->klass->extensions[j]->uuid))
            {
                return AUTK_ERR_DUPLICATE_EXTENSION;
            }
        }
    }

    // Compute the size and layout of the style object.
    class_data_offset = alloc_size;
    if (params->klass->class_data_size > 0) {
        class_data_size = autk_align_up(params->klass->class_data_size);
        if (!class_data_size || class_data_size > SIZE_MAX - alloc_size) {
            return AUTK_ERR_ARITHMETIC_OVERFLOW;
        }
        alloc_size += class_data_size;
    }

    user_data_offset = alloc_size;
    if (params->user_data_size > 0) {
        user_data_size = autk_align_up(params->user_data_size);
        if (!user_data_size || user_data_size > SIZE_MAX - alloc_size) {
            return AUTK_ERR_ARITHMETIC_OVERFLOW;
        }
        alloc_size += user_data_size;
    }

    // Allocate the style object and initialize the header.
    style = autk_instance_alloc(instance, NULL, 0, alloc_size, AUTK_MEMORY_TAG_STYLE);
    if (!style) {
        return AUTK_ERR_OUT_OF_MEMORY;
    }

    *style = (autk_style_t){
        .klass = params->klass,
        .ref_count = 1,
        .alloc_size = alloc_size,
        .instance = instance,
        .class_data = class_data_size ? (char *)style + class_data_offset : NULL,
        .user_data = user_data_size ? (char *)style + user_data_offset : NULL,
    };

    // Initialize the class data.
    if (params->klass->init) {
        status = params->klass->init(style, style->class_data, params->class_init_ctx);
        if (status != AUTK_OK) {
            if (params->klass->fini) {
                params->klass->fini(style, style->class_data);
            }
            autk_instance_alloc(instance, style, style->alloc_size, 0, AUTK_MEMORY_TAG_STYLE);
            return status;
        }
    } else if (params->klass->class_data_size > 0) {
        memset(style->class_data, 0, params->klass->class_data_size);
    }

    // Initialize the user data.
    if (params->user_data_size > 0) {
        if (params->user_data_init) {
            memcpy(style->user_data, params->user_data_init, params->user_data_size);
        } else {
            memset(style->user_data, 0, params->user_data_size);
        }
    }

    *out_style = style;
    return AUTK_OK;
}

AUTK_API autk_style_t *
autk_style_retain(autk_style_t *style)
{
    if (!style) {
        return NULL;
    }

    atomic_fetch_add_explicit(&style->ref_count, 1, memory_order_relaxed);
    return style;
}

AUTK_API void
autk_style_release(autk_style_t *style)
{
    if (!style) {
        return;
    }

    if (atomic_fetch_sub_explicit(&style->ref_count, 1, memory_order_acquire) == 1) {
        atomic_thread_fence(memory_order_release);
        if (style->klass->fini) {
            style->klass->fini(style, style->class_data);
        }
        autk_instance_alloc(style->instance, style, style->alloc_size, 0, AUTK_MEMORY_TAG_STYLE);
    }
}

AUTK_API void *
autk_style_get_class_data(autk_style_t *style)
{
    return style ? style->class_data : NULL;
}

AUTK_API autk_instance_t *
autk_style_get_instance(autk_style_t *style)
{
    return style ? style->instance : NULL;
}

AUTK_API void *
autk_style_get_user_data(autk_style_t *style)
{
    return style ? style->user_data : NULL;
}

AUTK_API const autk_extension_header_t *
autk_style_find_extension(autk_style_t *style, const autk_extension_query_t *query)
{
    const autk_extension_header_t *ext;

    if (!style || !query) {
        return NULL;
    }

    for (size_t i = 0; i < style->klass->extension_count; i++) {
        ext = style->klass->extensions[i];
        if (autk_uuid_equals(&ext->uuid, &query->uuid) && ext->struct_size >= query->min_struct_size
            && ext->version >= query->min_version)
        {
            return ext;
        }
    }

    return NULL;
}

AUTK_API bool
autk_style_class_provides_extension(const autk_style_class_t *klass,
                                    const autk_extension_query_t *query)
{
    const autk_extension_header_t *ext;

    if (!klass || !query || klass->struct_size != sizeof(autk_style_class_t) || !klass->extensions)
    {
        return false;
    }

    for (uint16_t i = 0; i < klass->extension_count; i++) {
        ext = klass->extensions[i];
        if (autk_uuid_equals(&ext->uuid, &query->uuid) && ext->struct_size >= query->min_struct_size
            && ext->version >= query->min_version)
        {
            return true;
        }
    }

    return false;
}
