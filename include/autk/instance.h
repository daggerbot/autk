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

AUTK_BEGIN_DECLS

/// Creates an Autk library instance. Nearly everything provided by Autk stems from an instance.
///
/// \param params Creation parameters. Can be `NULL` to use defaults.
/// \param out_instance Where to store the created instance. Must not be `NULL`. Updated on success,
///                     or left unchanged on failure.
/// \return `AUTK_OK` on success, or an error code on failure.
AUTK_API autk_status_t
autk_instance_create(const autk_instance_create_params_t *params, autk_instance_t **out_instance);

/// Destroys an Autk library instance. The caller is responsible for ensuring that all resources
/// created from the instance have been destroyed before calling this function.
AUTK_API void
autk_instance_destroy(autk_instance_t *instance);

/// Returns a pointer to user data associated with the instance, or `NULL` if no user data was
/// allocated.
AUTK_API void *
autk_instance_get_user_data(autk_instance_t *instance);

/// Invokes the instance's message handler, if one was provided at instance creation.
///
/// Calling this manually is inconvenient due to the sheer number of parameters.
/// Consider using the \ref AUTK_ERROR, \ref AUTK_WARN, and \ref AUTK_INFO macros instead.
///
/// \param instance The instance whose message handler to invoke. Must not be `NULL`.
/// \param severity The severity of the message.
/// \param module_name The name of the module that generated the message. Can be `NULL`.
/// \param location The source location where the message was generated. Can be `NULL`.
/// \param message The message to log. Must not be `NULL`.
AUTK_API void
autk_instance_message(const autk_instance_t *instance, autk_message_severity_t severity,
                      const char *module_name, const autk_source_location_t *location,
                      const char *message);

/// A printf-style version of \ref autk_instance_message.
///
/// \return `false` if formatting the message failed, otherwise `true`. Other failures such as no
///         message handler registered are not considered errors and will not cause this function to
///         return `false`. This gives the caller the opportunity to provide an unformatted fallback
///         message if formatting fails.
AUTK_API bool
autk_instance_message_f(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, ...) AUTK_FMT(5, 6);

/// A vprintf-style version of \ref autk_instance_message.
///
/// \return `false` if formatting the message failed, otherwise `true`. Other failures such as no
///         message handler registered are not considered errors and will not cause this function to
///         return `false`. This gives the caller the opportunity to provide an unformatted fallback
///         message if formatting fails.
AUTK_API bool
autk_instance_message_v(const autk_instance_t *instance, autk_message_severity_t severity,
                        const char *module_name, const autk_source_location_t *location,
                        const char *fmt, va_list args) AUTK_VFMT(5, 6);

/// Invokes the instance's memory allocator to allocate, reallocate, or deallocate memory.
///
/// **Example usage:**
/// \code
/// // Allocate 256 bytes of memory for a string.
/// char *str = autk_instance_alloc(instance, NULL, 0, 256, AUTK_MEMORY_TAG_STRING);
/// if (!str) {
///     return AUTK_ERR_OUT_OF_MEMORY;
/// }
///
/// // Reallocate the string to 512 bytes.
/// str = autk_instance_alloc(instance, str, 256, 512, AUTK_MEMORY_TAG_STRING);
/// if (!str) {
///     return AUTK_ERR_OUT_OF_MEMORY;
/// }
///
/// // Free the string.
/// autk_instance_alloc(instance, str, 512, 0, AUTK_MEMORY_TAG_STRING);
/// \endcode
///
/// \param instance The instance whose memory allocator to invoke. Must not be `NULL`.
/// \param mem The memory to reallocate or deallocate, or `NULL` to allocate new memory.
/// \param old_size The size of the memory pointed to by `mem`, or `0` if `mem` is `NULL`.
///                 When reallocating or deallocating, this must be the exact size that was
///                 previously allocated.
/// \param new_size The size of the memory to allocate or reallocate to, or `0` to deallocate `mem`.
/// \param tag The memory tag associated with the allocation, reallocation, or deallocation.
///            The allocator may use this for any reason, such as debugging or choosing different
///            allocation strategies. When reallocating or deallocating, this must be the same tag
///            that was previously used for the allocation.
/// \return A pointer to the allocated or reallocated memory, or `NULL` if the allocation or
///         reallocation failed. When deallocating, this return value should be ignored, as
///         deallocation is assumed to always succeed.
AUTK_API void *
autk_instance_alloc(const autk_instance_t *instance, void *mem, size_t old_size, size_t new_size,
                    autk_memory_tag_t tag);

/// The default allcator function used when creating an instance with `NULL` for the allocator.
/// Defers to the standard `malloc`, `realloc`, and `free` functions. Ignores `ctx`, `old_size`,
/// and `tag`.
AUTK_API void *
autk_default_alloc(void *ctx, void *mem, size_t old_size, size_t new_size, autk_memory_tag_t tag);

AUTK_END_DECLS

#endif // AUTK_INSTANCE_H_
