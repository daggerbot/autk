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

#ifndef AUTK_DIAGNOSTICS_H_
#define AUTK_DIAGNOSTICS_H_

#include "instance.h"

/// \def AUTK_DEBUG_ENABLED
/// Defined as 0 or 1 to control whether \ref AUTK_DEBUG messages are reported.
/// Enabled by default if `NDEBUG` is not defined.
#ifndef AUTK_DEBUG_ENABLED
# ifdef NDEBUG
#  define AUTK_DEBUG_ENABLED 0
# else
#  define AUTK_DEBUG_ENABLED 1
# endif
#endif

/// \def AUTK_MESSAGE_SOURCE_LOCATION
/// Defined as 0 or 1 to control whether the messaging macros should include a source location.
#ifndef AUTK_MESSAGE_SOURCE_LOCATION
# define AUTK_MESSAGE_SOURCE_LOCATION 0
#endif

/// \def AUTK_MODULE_NAME
/// If defined as a string, this module name is included in messages when using the messaging
/// macros. Default value is `NULL`.
#ifndef AUTK_MODULE_NAME
# define AUTK_MODULE_NAME NULL
#endif

#ifndef AUTK_TOPLEVEL
# ifdef __cplusplus
#  define AUTK_TOPLEVEL ::
# else
#  define AUTK_TOPLEVEL
# endif
#endif

/// \def AUTK_DEBUG
/// Reports a message with \ref AUTK_MESSAGE_SEVERITY_DEBUG.
/// No-op if \ref AUTK_DEBUG_ENABLED is 0.
/// Includes the source location if \ref AUTK_MESSAGE_SOURCE_LOCATION is 1.
#if AUTK_DEBUG_ENABLED
# if AUTK_MESSAGE_SOURCE_LOCATION
#  define AUTK_DEBUG(instance, ...)                                                                \
      do {                                                                                         \
          static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                               __LINE__};          \
          AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_DEBUG,           \
                                                AUTK_MODULE_NAME, &_autk_location_, __VA_ARGS__);  \
      } while (0)
# else
#  define AUTK_DEBUG(instance, ...)                                                                \
      AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_DEBUG,               \
                                            AUTK_MODULE_NAME, NULL, __VA_ARGS__)
# endif
#else
# define AUTK_DEBUG(instance, ...) AUTK_TOPLEVEL autk_message_nop((instance), __VA_ARGS__)
#endif

/// \def AUTK_WARN
/// Reports a message with \ref AUTK_MESSAGE_SEVERITY_WARN.
/// Includes the source location if \ref AUTK_MESSAGE_SOURCE_LOCATION is 1.
#if AUTK_MESSAGE_SOURCE_LOCATION
# define AUTK_WARN(instance, ...)                                                                  \
     do {                                                                                          \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__,  \
                                                                              __LINE__};           \
         AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_WARN,             \
                                               AUTK_MODULE_NAME, &_autk_location_, __VA_ARGS__);   \
     } while (0)
#else
# define AUTK_WARN(instance, ...)                                                                  \
     AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_WARN,                 \
                                           AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_ERROR
/// Reports a message with \ref AUTK_MESSAGE_SEVERITY_ERROR.
/// Includes the source location if \ref AUTK_MESSAGE_SOURCE_LOCATION is 1.
#if AUTK_MESSAGE_SOURCE_LOCATION
# define AUTK_ERROR(instance, ...)                                                                 \
     do {                                                                                          \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__,  \
                                                                              __LINE__};           \
         AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_ERROR,            \
                                               AUTK_MODULE_NAME, &_autk_location_, __VA_ARGS__);   \
     } while (0)
#else
# define AUTK_ERROR(instance, ...)                                                                 \
     AUTK_TOPLEVEL autk_instance_message_f((instance), AUTK_MESSAGE_SEVERITY_ERROR,                \
                                           AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_EXPECT
/// Assertion macro for functions that return \ref autk_status_t.
///
/// If the expression does not evaluate to \ref AUTK_OK, this function calls the user-defined
/// \ref autk_expect_failed, which is responsible for reporting the error and terminating the
/// process.
#if AUTK_MESSAGE_SOURCE_LOCATION
# define AUTK_EXPECT(expr)                                                                         \
     do {                                                                                          \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__,  \
                                                                              __LINE__};           \
         AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr);                                       \
         if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) {                                             \
             AUTK_TOPLEVEL autk_expect_failed(#expr, _autk_status_, AUTK_MODULE_NAME,              \
                                              &_autk_location_);                                   \
         }                                                                                         \
     } while (0)
#else
# define AUTK_EXPECT(expr)                                                                         \
     do {                                                                                          \
         AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr);                                       \
         if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) {                                             \
             AUTK_TOPLEVEL autk_expect_failed(#expr, _autk_status_, AUTK_MODULE_NAME, NULL);       \
         }                                                                                         \
     } while (0)
#endif

/// \def AUTK_TRY
/// Macro for expressions that evaluate to \ref autk_status_t. Immediately returns from the current
/// function with the status if it is not `AUTK_OK`.
#define AUTK_TRY(expr)                                                                             \
    do {                                                                                           \
        AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr);                                        \
        if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) {                                              \
            return _autk_status_;                                                                  \
        }                                                                                          \
    } while (0)

AUTK_BEGIN_DECLS

/// No-op function that can be used by macros like \ref AUTK_DEBUG when disabled.
AUTK_FMT(2, 3) static AUTK_ALWAYS_INLINE void
autk_message_nop(autk_instance_t *instance, const char *fmt, ...)
{
    (void)instance;
    (void)fmt;
}

/// A sensible default implementation for \ref autk_expect_failed. Writes to stderr, shows an error
/// dialog (if the platform natively supports this), and terminates the process with `EXIT_FAILURE`.
AUTK_API AUTK_NORETURN void
autk_default_expect_failed(const char *expr, autk_status_t status, const char *module_name,
                           const autk_source_location_t *location);

/// User-defined function for handling \ref AUTK_EXPECT failure.
///
/// This function typically reports the error and terminates the process. A sensible default
/// implementation \ref autk_default_expect_failed is provided.
AUTK_NORETURN void
autk_expect_failed(const char *expr, autk_status_t status, const char *module_name,
                   const autk_source_location_t *location);

AUTK_API size_t
autk_status_to_string(autk_status_t status, char *buf, size_t buf_size);

AUTK_API void
autk_stderr_message(void *unused, autk_message_severity_t severity, const char *module_name,
                    const autk_source_location_t *location, const char *message);

AUTK_END_DECLS

#endif // AUTK_DIAGNOSTICS_H_
