// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <stdio.h>
#include <stdlib.h>

#include "string_utils.h"

AUTK_HIDDEN autk_status_t
autk_call_with_formatted_string_f(autk_fmt_flags_t flags,
                                  void (*callback)(void *ctx, const char *str, size_t len),
                                  void *ctx, const char *fmt, ...)
{
    va_list args;
    autk_status_t status;

    va_start(args, fmt);
    status = autk_call_with_formatted_string_vf(flags, callback, ctx, fmt, args);
    va_end(args);
    return status;
}

AUTK_HIDDEN autk_status_t
autk_call_with_formatted_string_vf(autk_fmt_flags_t flags,
                                   void (*callback)(void *ctx, const char *str, size_t len),
                                   void *ctx, const char *fmt, va_list args)
{
    char stack_buf[512];
    autk_status_t status;
    va_list args2;
    int result, result2;
    size_t buf_size;
    char *heap_buf;

    // So we can call vsnprintf() twice if needed.
    va_copy(args2, args);

    // Try formatting to the stack buffer first.
    result = vsnprintf(stack_buf, sizeof(stack_buf), fmt, args);
    if (result < 0) {
        status = AUTK_ERR_STRING_FORMAT;
        goto exit_va_end;
    } else if ((size_t)result < sizeof(stack_buf)) {
        // Formatting succeeded without truncation.
        // This is the best case scenario.
        callback(ctx, stack_buf, (size_t)result);
        status = AUTK_OK;
        goto exit_va_end;
    } else if (!(flags & AUTK_FMT_ALLOCATE)) {
        // String was truncated and heap is not allowed.
        if (flags & AUTK_FMT_ALLOW_TRUNCATE) {
            callback(ctx, stack_buf, sizeof(stack_buf) - 1);
            status = AUTK_OK;
        } else {
            status = AUTK_ERR_LIMIT_EXCEEDED;
        }
        goto exit_va_end;
    }

    // If we reach here, the formatted string exceeds the stack buffer.
    // We have to allocate a heap buffer with the correct size and try again.
    buf_size = (size_t)result + 1;
    heap_buf = malloc(buf_size);
    if (!heap_buf) {
        if (flags & AUTK_FMT_ALLOW_TRUNCATE) {
            callback(ctx, stack_buf, sizeof(stack_buf) - 1);
            status = AUTK_OK;
        } else {
            status = AUTK_ERR_OUT_OF_MEMORY;
        }
        goto exit_va_end;
    }

    result2 = vsnprintf(heap_buf, buf_size, fmt, args2);
    if (result2 < 0) {
        if (flags & AUTK_FMT_ALLOW_TRUNCATE) {
            callback(ctx, stack_buf, sizeof(stack_buf) - 1);
            status = AUTK_OK;
        } else {
            status = AUTK_ERR_STRING_FORMAT;
        }
        goto exit_free_heap_buf;
    } else if (result2 != result) {
        status = AUTK_ERR_INCONSISTENT_STATE;
        goto exit_free_heap_buf;
    }

    callback(ctx, heap_buf, (size_t)result2);
    status = AUTK_OK;

exit_free_heap_buf:
    free(heap_buf);
exit_va_end:
    va_end(args2);
    return status;
}

AUTK_HIDDEN char *
autk_strcpy_max(char *AUTK_RESTRICT buf, const char *AUTK_RESTRICT src, size_t buf_size)
{
    size_t i;

    if (buf_size) {
        for (i = 0; i + 1 < buf_size; i++) {
            buf[i] = src[i];
        }
        buf[i] = 0;
    }

    return buf;
}
