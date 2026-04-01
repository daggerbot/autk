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

typedef struct autk_client autk_client_t;
typedef struct autk_client_create_params autk_client_create_params_t;
typedef struct autk_client_driver autk_client_driver_t;
typedef struct autk_instance autk_instance_t;
typedef struct autk_instance_create_params autk_instance_create_params_t;
typedef struct autk_lifetime_hooks autk_lifetime_hooks_t;
typedef struct autk_job autk_job_t;
typedef struct autk_source_location autk_source_location_t;
typedef struct autk_window autk_window_t;
typedef struct autk_window_callbacks autk_window_callbacks_t;
typedef struct autk_window_create_params autk_window_create_params_t;
typedef struct autk_window_driver autk_window_driver_t;

//==============================================================================
//
// Enums
//
//==============================================================================

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

enum autk_memory_tag {
/* clang-format off */
#define AUTK_FOREACH_MEMORY_TAG(m) \
    m(AUTK_MEMORY_TAG_UNKNOWN, "unknown") \
    m(AUTK_MEMORY_TAG_CLIENT, "client") \
    m(AUTK_MEMORY_TAG_HASH, "hash") \
    m(AUTK_MEMORY_TAG_INSTANCE, "instance") \
    m(AUTK_MEMORY_TAG_QUEUE, "queue") \
    m(AUTK_MEMORY_TAG_STRING, "string") \
    m(AUTK_MEMORY_TAG_WINDOW, "window")
/* clang-format on */
#define AUTK_DO(e, s) e,
    AUTK_FOREACH_MEMORY_TAG(AUTK_DO)
#undef AUTK_DO
};

enum autk_message_severity {
    AUTK_MESSAGE_SEVERITY_DEBUG = 1,
    AUTK_MESSAGE_SEVERITY_WARN,
    AUTK_MESSAGE_SEVERITY_ERROR,
    AUTK_MESSAGE_SEVERITY_FATAL,
};

typedef enum autk_object_type {
    AUTK_OBJECT_TYPE_NONE,
    AUTK_OBJECT_TYPE_CLIENT,
    AUTK_OBJECT_TYPE_INSTANCE,
    AUTK_OBJECT_TYPE_WINDOW,
} autk_object_type_t;

enum autk_status {
/* clang-format off */
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
    m(AUTK_ERR_UNSUPPORTED_EXTENSION, "Unsupported extension") \
    m(AUTK_ERR_UNSUPPORTED_FEATURE, "Unsupported feature") \
    m(AUTK_ERR_WOULD_BLOCK, "Operation would block")
/* clang-format on */
#define AUTK_DO(e, s) e,
    AUTK_FOREACH_STATUS(AUTK_DO)
#undef AUTK_DO
};

enum autk_window_create_flags {
    /// An explicit position is provided in the create params. If this flag is not set, the window
    /// system will choose a position for the window.
    AUTK_WINDOW_CREATE_FLAGS_POSITION = 1 << 0,
    /// An explicit size is provided in the create params. If this flag is not set, the window
    /// system will choose a size for the window.
    AUTK_WINDOW_CREATE_FLAGS_SIZE = 1 << 1,

    AUTK_WINDOW_CREATE_FLAGS_ALL =
        AUTK_WINDOW_CREATE_FLAGS_POSITION | AUTK_WINDOW_CREATE_FLAGS_SIZE,

    AUTK_WINDOW_CREATE_FLAGS_32BIT_ = 0x7FFFFFFFul,
};

enum autk_window_type {
    AUTK_WINDOW_TYPE_NORMAL,
};

typedef enum autk_memory_tag autk_memory_tag_t;
typedef enum autk_message_severity autk_message_severity_t;
typedef enum autk_status autk_status_t;
typedef enum autk_window_type autk_window_type_t;

typedef uint32_t autk_instance_flags_t; ///< \see \ref autk_instance_flags
typedef uint32_t autk_window_create_flags_t; ///< \see \ref autk_window_create_flags

//==============================================================================
//
// Function pointers
//
//==============================================================================

/// Allocator function type. Handles allocation, reallocation, and deallocation.
///
/// Deallocation (`new_size == 0`) is assumed to always succeed.
///
/// \param ctx User-provided context pointer passed to the allocator on each call.
/// \param mem Pointer to the memory block to reallocate or free, or `NULL` to allocate a new block.
/// \param old_size Size of the memory block pointed to by `mem`, or `0` if `mem` is `NULL`.
/// \param new_size Size of the new memory block to allocate or reallocate to, or `0` to free the
///                 block pointed to by `mem`.
/// \param tag Hint passed to the allocator describing the type of data being allocated.
///            The allocator may use this information for any reason.
///            Most allocators will likely ignore this.
///
/// \return Pointer to the allocated or reallocated memory block, or `NULL` if the allocation
///         failed.
typedef void *(*autk_alloc_func_t)(void *ctx, void *mem, size_t old_size, size_t new_size,
                                   autk_memory_tag_t tag);

/// Message callback function type.
///
/// \param ctx User-provided context pointer passed to the message callback on each call.
/// \param severity Severity level of the message.
/// \param module_name Name of the module that generated the message, or `NULL` if none was
///                    provided.
/// \param location Source code location where the message was generated, or `NULL` if none was
///                 provided.
/// \param message The message string. The encoding of this string is assumed to be UTF-8, but this
///                is not strictly enforced internally.
typedef void (*autk_message_func_t)(void *ctx, autk_message_severity_t severity,
                                    const char *module_name, const autk_source_location_t *location,
                                    const char *message);

//==============================================================================
//
// Structs
//
//==============================================================================

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

struct autk_instance_create_params {
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
};

struct autk_job {
    void *ctx;
    void (*exec)(void *ctx, autk_client_t *client);
    void (*fini)(void *ctx);
};

struct autk_lifetime_hooks {
    void (*created)(void *ctx, autk_instance_t *instance, void *child,
                    autk_object_type_t child_type, void *parent, autk_object_type_t parent_type);
    void (*destroying)(void *ctx, autk_instance_t *instance, void *child,
                       autk_object_type_t child_type, void *parent, autk_object_type_t parent_type);
    void (*reparenting)(void *ctx, autk_instance_t *instance, void *child,
                        autk_object_type_t child_type, void *old_parent,
                        autk_object_type_t old_parent_type, void *new_parent,
                        autk_object_type_t new_parent_type);
};

struct autk_source_location {
    const char *func_name;
    const char *file_name;
    int line_num;
};

struct autk_window_callbacks {
    size_t struct_size;
    /// Invoked when the window is destroyed via \ref autk_window_destroy.
    void (*destroying)(autk_window_t *window, void *user_data);
    /// Invoked when a window destroy event is received from the window system without a
    /// corresponding \ref autk_window_destroy call.
    void (*invalidated)(autk_window_t *window, void *user_data);
    /// Invoked when the user or window system sends a window close request.
    void (*close_requested)(autk_window_t *window, void *user_data);
};

struct autk_window_create_params {
    size_t struct_size;
    autk_window_type_t type;
    autk_window_create_flags_t flags;
    const char *title;
    int32_t x, y;
    uint32_t width, height;
    const autk_window_callbacks_t *callbacks;
    size_t user_data_size;
    const void *user_data_init;
};

struct autk_window_driver {
    size_t struct_size;
    size_t driver_data_size;

    autk_status_t (*init)(autk_window_t *window, void *driver_data,
                          const autk_window_create_params_t *params);
    void (*fini)(autk_window_t *window, void *driver_data);
    autk_status_t (*set_title)(autk_window_t *window, void *driver_data, const char *title);
    autk_status_t (*set_visible)(autk_window_t *window, void *driver_data, bool visible);
};

#endif // AUTK_TYPES_H_
