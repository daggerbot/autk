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

#ifndef AUTK_TYPES_H_
#define AUTK_TYPES_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <autk/config.h>

typedef uint64_t autk_uid_t;

typedef struct autk_client autk_client_t;
typedef struct autk_instance autk_instance_t;
typedef struct autk_window autk_window_t;

typedef enum autk_memory_tag {
    AUTK_MEMORY_TAG_UNKNOWN,
    AUTK_MEMORY_TAG_CLIENT,
    AUTK_MEMORY_TAG_HASH,
    AUTK_MEMORY_TAG_INSTANCE,
    AUTK_MEMORY_TAG_QUEUE,
    AUTK_MEMORY_TAG_STRING,
    AUTK_MEMORY_TAG_WINDOW,
} autk_memory_tag_t;

typedef enum autk_message_severity {
    AUTK_MESSAGE_SEVERITY_DEBUG = 1,
    AUTK_MESSAGE_SEVERITY_WARN,
    AUTK_MESSAGE_SEVERITY_ERROR,
    AUTK_MESSAGE_SEVERITY_FATAL,
} autk_message_severity_t;

typedef enum autk_object_type {
    AUTK_OBJECT_TYPE_NONE,
    AUTK_OBJECT_TYPE_CLIENT,
    AUTK_OBJECT_TYPE_INSTANCE,
    AUTK_OBJECT_TYPE_WINDOW,
} autk_object_type_t;

typedef enum autk_status {
// clang-format off
#define AUTK_FOREACH_STATUS(m) \
    m(AUTK_OK, "No error") \
    m(AUTK_ERR_ARITHMETIC_OVERFLOW, "Arithmetic overflow") \
    m(AUTK_ERR_DATA_CORRUPTION, "Data corruption detected") \
    m(AUTK_ERR_INVALID_ARGUMENT, "Invalid argument") \
    m(AUTK_ERR_INVALID_CONFIGURATION, "Invalid configuration") \
    m(AUTK_ERR_INVALID_STRING_ENCODING, "Invalid string encoding") \
    m(AUTK_ERR_INVALID_STRUCT_SIZE, "Invalid struct size") \
    m(AUTK_ERR_INSUFFICIENT_BUFFER, "Insufficient buffer size") \
    m(AUTK_ERR_INTERRUPTED, "Operation interrupted") \
    m(AUTK_ERR_IO_FAILURE, "I/O failure") \
    m(AUTK_ERR_OUT_OF_MEMORY, "Out of memory") \
    m(AUTK_ERR_PROTOCOL_VIOLATION, "Protocol violation") \
    m(AUTK_ERR_QUEUE_EMPTY, "Queue is empty") \
    m(AUTK_ERR_QUEUE_FULL, "Queue is full") \
    m(AUTK_ERR_REQUEST_DENIED, "Request denied") \
    m(AUTK_ERR_RESOURCE_LOST, "Resource lost") \
    m(AUTK_ERR_RUNTIME_FAILURE, "Runtime failure") \
    m(AUTK_ERR_SYNC_FAILURE, "Synchronization failure") \
    m(AUTK_ERR_TIMEOUT, "Operation timed out") \
    m(AUTK_ERR_TRY_AGAIN, "Try again later") \
    m(AUTK_ERR_UNIMPLEMENTED, "Not implemented") \
    m(AUTK_ERR_UNSUPPORTED_EXTENSION, "Unsupported extension")
// clang-format on
#define AUTK_DO(e, s) e,
    AUTK_FOREACH_STATUS(AUTK_DO)
#undef AUTK_DO
} autk_status_t;

typedef struct autk_job {
    void *ctx;
    void (*exec)(void *ctx, autk_client_t *client);
    void (*fini)(void *ctx);
} autk_job_t;

typedef struct autk_source_location {
    const char *func_name;
    const char *file_name;
    int line_num;
} autk_source_location_t;

#endif // AUTK_TYPES_H_
