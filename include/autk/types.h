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

#define AUTK_LENGTHOF(a) (sizeof(a) / sizeof((a)[0]))
#define AUTK_RGB_INIT(R, G, B) {.r = (R), .g = (G), .b = (B), .a = 255}
#define AUTK_RGB(R, G, B) ((autk_rgb_t)AUTK_RGB_INIT(R, G, B))
#define AUTK_RGBA_INIT(R, G, B, A) {.r = (R), .g = (G), .b = (B), .a = (A)}
#define AUTK_RGBA(R, G, B, A) ((autk_rgba_t)AUTK_RGBA_INIT(R, G, B, A))

//==============================================================================
//
// Common types
//
//==============================================================================

typedef uint32_t autk_u30x2_t; ///< Unsigned 30.2 fixed-point.

typedef enum autk_status {
/* clang-format off */
#define AUTK_FOREACH_STATUS(m) \
    m(AUTK_OK, "No error") \
    m(AUTK_ERR_ARITHMETIC_OVERFLOW, "Arithmetic overflow") \
    m(AUTK_ERR_DATA_CORRUPTION, "Data corruption detected") \
    m(AUTK_ERR_DUPLICATE_EXTENSION, "Duplicate extension") \
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
} autk_status_t;

typedef struct autk_rect {
    int32_t x, y;
    uint32_t width, height;
} autk_rect_t;

typedef struct autk_bbox {
    int32_t x0, y0, x1, y1;
} autk_bbox_t;

typedef union autk_rgba {
    struct {
        uint8_t r, g, b, a;
    };
    uint32_t value;
} autk_rgb_t, autk_rgba_t;

typedef union autk_uuid {
    uint8_t bytes[16];
    uint64_t parts[2];
} autk_uuid_t;

typedef struct autk_extension_header {
    uint32_t struct_size;
    autk_uuid_t uuid;
    uint32_t version;
} autk_extension_header_t;

typedef struct autk_extension_query {
    autk_uuid_t uuid;
    uint32_t min_struct_size;
    uint32_t min_version;
} autk_extension_query_t;

//==============================================================================
//
// Memory management types
//
//==============================================================================

typedef enum autk_memory_tag {
/* clang-format off */
#define AUTK_FOREACH_MEMORY_TAG(m) \
    m(AUTK_MEMORY_TAG_UNKNOWN, "unknown") \
    m(AUTK_MEMORY_TAG_CLIENT, "client") \
    m(AUTK_MEMORY_TAG_HASH, "hash") \
    m(AUTK_MEMORY_TAG_INSTANCE, "instance") \
    m(AUTK_MEMORY_TAG_LIST, "list") \
    m(AUTK_MEMORY_TAG_QUEUE, "queue") \
    m(AUTK_MEMORY_TAG_STRING, "string") \
    m(AUTK_MEMORY_TAG_STYLE, "style") \
    m(AUTK_MEMORY_TAG_WINDOW, "window")
/* clang-format on */
#define AUTK_DO(e, s) e,
    AUTK_FOREACH_MEMORY_TAG(AUTK_DO)
#undef AUTK_DO
} autk_memory_tag_t;

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

//==============================================================================
//
// Diagnostics types
//
//==============================================================================

typedef enum autk_message_severity {
    AUTK_MESSAGE_SEVERITY_DEBUG = 1,
    AUTK_MESSAGE_SEVERITY_WARN,
    AUTK_MESSAGE_SEVERITY_ERROR,
    AUTK_MESSAGE_SEVERITY_FATAL,
} autk_message_severity_t;

typedef struct autk_source_location {
    const char *func_name;
    const char *file_name;
    uint32_t line_num;
} autk_source_location_t;

//==============================================================================
//
// Instance types
//
//==============================================================================

typedef struct autk_instance autk_instance_t;

enum autk_instance_create_flags {
    /// Indicates that the provided allocator is thread-safe.
    /// If not, some features may be unavailable.
    AUTK_INSTANCE_CREATE_FLAG_THREAD_SAFE_ALLOC = 1 << 0,
    /// Indicates that the provided message hook is thread-safe.
    /// If not, some messages may be skipped.
    AUTK_INSTANCE_CREATE_FLAG_THREAD_SAFE_MESSAGE = 1 << 1,

    AUTK_INSTANCE_CREATE_FLAG_ALL =
        AUTK_INSTANCE_CREATE_FLAG_THREAD_SAFE_ALLOC | AUTK_INSTANCE_CREATE_FLAG_THREAD_SAFE_MESSAGE,

    AUTK_INSTANCE_32BIT_ = 0x7FFFFFFFul,
};
typedef uint32_t autk_instance_create_flags_t; ///< \see \ref autk_instance_create_flags

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

typedef struct autk_instance_create_params {
    uint32_t struct_size;
    enum autk_instance_create_flags flags;
    autk_alloc_func_t alloc_func;
    void *alloc_ctx;
    autk_message_func_t message_func;
    void *message_ctx;
    uint32_t user_data_size;
    const void *user_data_init;
} autk_instance_create_params_t;

//==============================================================================
//
// Client types
//
//==============================================================================

typedef struct autk_client autk_client_t;

typedef struct autk_client_callbacks {
    uint32_t struct_size;
    /// Invoked when the event queue has been fully consumed and the client is about to block
    /// waiting for new events.
    void (*begin_wait)(struct autk_client *client, void *user_data);
} autk_client_callbacks_t;

typedef struct autk_client_create_params {
    uint32_t struct_size;
    const struct autk_client_driver *driver;
    const char *display_name;
    void *driver_init_ctx;
    uint16_t app_style_count;
    struct autk_style *const *app_styles;
    const autk_client_callbacks_t *callbacks;
    uint32_t user_data_size;
    const void *user_data_init;
} autk_client_create_params_t;

typedef struct autk_job {
    void *ctx;
    void (*exec)(void *ctx, struct autk_client *client);
    void (*fini)(void *ctx);
} autk_job_t;

typedef struct autk_client_driver {
    /// Size of this struct. Must be `sizeof(autk_client_driver_t)`.
    uint32_t struct_size;
    /// Number of bytes to allocate for driver use.
    uint32_t driver_data_size;

    const struct autk_device_driver *device_driver;
    const struct autk_window_driver *window_driver;

    /// Function called to initialize the client's driver data after is it allocated.
    autk_status_t (*init)(struct autk_client *client, void *driver_data,
                          const struct autk_client_create_params *params);
    /// Function called to clean up the client's driver data.
    void (*fini)(struct autk_client *client, void *driver_data);
    autk_status_t (*run)(struct autk_client *client, void *driver_data);
    autk_status_t (*quit)(struct autk_client *client, void *driver_data);
} autk_client_driver_t;

//==============================================================================
//
// Device types
//
//==============================================================================

typedef struct autk_device autk_device_t;

typedef struct autk_device_driver {
    uint32_t struct_size;
    uint32_t driver_data_size;

    autk_status_t (*init_from_client)(autk_device_t *device, void *driver_data,
                                      autk_client_t *client, void *client_driver_data,
                                      const autk_client_create_params_t *client_params);
    void (*fini)(autk_device_t *device, void *driver_data);
} autk_device_driver_t;

//==============================================================================
//
// Style types
//
//==============================================================================

typedef struct autk_style autk_style_t;

typedef struct autk_style_class {
    uint32_t struct_size;
    uint32_t class_data_size;
    uint16_t extension_count;
    const autk_extension_header_t *const *extensions;
    autk_status_t (*init)(autk_style_t *style, void *style_data, const void *class_init_ctx);
    void (*fini)(autk_style_t *style, void *style_data);
} autk_style_class_t;

typedef struct autk_style_create_params {
    uint32_t struct_size;
    const autk_style_class_t *klass;
    const void *class_init_ctx; // class-specific initialization data
    uint32_t user_data_size;
    const void *user_data_init;
} autk_style_create_params_t;

//==============================================================================
//
// Window types
//
//==============================================================================

typedef struct autk_window autk_window_t;

struct autk_window_create_params;

enum autk_window_create_flags {
    /// An explicit position is provided in the create params. If this flag is not set, the window
    /// system will choose a position for the window.
    AUTK_WINDOW_CREATE_FLAG_POSITION = 1 << 0,
    /// An explicit size is provided in the create params. If this flag is not set, the window
    /// system will choose a size for the window.
    AUTK_WINDOW_CREATE_FLAG_SIZE = 1 << 1,
    AUTK_WINDOW_CREATE_FLAG_BACKGROUND_COLOR = 1 << 2,

    AUTK_WINDOW_CREATE_FLAG_ALL = AUTK_WINDOW_CREATE_FLAG_POSITION | AUTK_WINDOW_CREATE_FLAG_SIZE
                                  | AUTK_WINDOW_CREATE_FLAG_BACKGROUND_COLOR,

    AUTK_WINDOW_CREATE_FLAG_32BIT_ = 0x7FFFFFFFul,
};
typedef uint32_t autk_window_create_flags_t; ///< \see \ref autk_window_create_flags

typedef enum autk_window_type {
    AUTK_WINDOW_TYPE_NORMAL,
} autk_window_type_t;

typedef struct autk_dirty_region {
    autk_bbox_t full_bbox;
    size_t partial_bbox_count;
    const autk_bbox_t *partial_bboxes;
} autk_dirty_region_t;

typedef struct autk_window_callbacks {
    uint32_t struct_size;
    /// Invoked when the window is destroyed via \ref autk_window_destroy.
    void (*destroying)(autk_window_t *window, void *user_data);
    /// Invoked when a window destroy event is received from the window system without a
    /// corresponding \ref autk_window_destroy call.
    void (*invalidated)(autk_window_t *window, void *user_data);
    /// Invoked when the user or window system sends a window close request.
    void (*close_requested)(autk_window_t *window, void *user_data);
    /// Invoked when the window's content needs to be redrawn.
    /// If the `dirty_region` parameter is `NULL`, the entire window needs to be redrawn.
    void (*redraw_requested)(autk_window_t *window, void *user_data,
                             const autk_dirty_region_t *dirty_region);
} autk_window_callbacks_t;

typedef struct autk_window_driver {
    uint32_t struct_size;
    uint32_t driver_data_size;

    autk_status_t (*init)(autk_window_t *window, void *driver_data,
                          const struct autk_window_create_params *params);
    void (*fini)(autk_window_t *window, void *driver_data);

    autk_status_t (*set_background_color)(autk_window_t *window, void *driver_data,
                                          autk_rgba_t color);
    autk_status_t (*set_title)(autk_window_t *window, void *driver_data, const char *title);
    autk_status_t (*set_visible)(autk_window_t *window, void *driver_data, bool visible);
} autk_window_driver_t;

typedef struct autk_window_create_params {
    uint32_t struct_size;
    autk_window_type_t type;
    autk_window_create_flags_t flags;
    const char *title;
    int32_t x, y;
    uint32_t width, height;
    autk_rgba_t background_color;
    const autk_window_callbacks_t *callbacks;
    uint32_t user_data_size;
    const void *user_data_init;
} autk_window_create_params_t;

#endif // AUTK_TYPES_H_
