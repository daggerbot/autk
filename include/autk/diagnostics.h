// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_MESSAGE_H_
#define AUTK_MESSAGE_H_

#include <inttypes.h>

#include "types.h"

#define AUTK_MESSAGE_SEVERITY_TRACE (-128)
#define AUTK_MESSAGE_SEVERITY_DEBUG (-8)
#define AUTK_MESSAGE_SEVERITY_INFO (-4)
#define AUTK_MESSAGE_SEVERITY_WARNING 4
#define AUTK_MESSAGE_SEVERITY_ERROR 8
#define AUTK_MESSAGE_SEVERITY_FATAL 127

#ifndef AUTK_ENABLE_ASSERT
# ifdef NDEBUG
#  define AUTK_ENABLE_ASSERT 0
# else
#  define AUTK_ENABLE_ASSERT 1
# endif
#endif

#ifndef AUTK_ENABLE_SOURCE_LOCATION
# ifdef NDEBUG
#  define AUTK_ENABLE_SOURCE_LOCATION 0
# else
#  define AUTK_ENABLE_SOURCE_LOCATION 1
# endif
#endif

#ifndef AUTK_MIN_MESSAGE_SEVERITY
# ifdef NDEBUG
#  define AUTK_MIN_MESSAGE_SEVERITY AUTK_MESSAGE_SEVERITY_INFO
# else
#  define AUTK_MIN_MESSAGE_SEVERITY AUTK_MESSAGE_SEVERITY_TRACE
# endif
#endif

#ifndef AUTK_MODULE_NAME
# define AUTK_MODULE_NAME NULL
#endif

/// \def AUTK_TRACE
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_TRACE
# define AUTK_TRACE(...) AUTK_TOPLEVEL autk_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_TRACE(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_TRACE, AUTK_MODULE_NAME, \
                                      &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_TRACE(...) \
     AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_TRACE, AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_DEBUG
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_DEBUG
# define AUTK_DEBUG(...) AUTK_TOPLEVEL autk_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_DEBUG(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_DEBUG, AUTK_MODULE_NAME, \
                                      &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_DEBUG(...) \
     AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_DEBUG, AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_INFO
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_INFO
# define AUTK_INFO(...) AUTK_TOPLEVEL autk_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_INFO(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_INFO, AUTK_MODULE_NAME, \
                                      &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_INFO(...) \
     AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_INFO, AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_WARNING
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_WARNING
# define AUTK_WARNING(...) AUTK_TOPLEVEL autk_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_WARNING(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_WARNING, AUTK_MODULE_NAME, \
                                      &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_WARNING(...) \
     AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_WARNING, AUTK_MODULE_NAME, NULL, \
                                  __VA_ARGS__)
#endif

/// \def AUTK_ERROR
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_ERROR
# define AUTK_ERROR(...) AUTK_TOPLEVEL autk_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_ERROR(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_ERROR, AUTK_MODULE_NAME, \
                                      &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_ERROR(...) \
     AUTK_TOPLEVEL autk_message_f(AUTK_MESSAGE_SEVERITY_ERROR, AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_FATAL
#if AUTK_MIN_MESSAGE_SEVERITY > AUTK_MESSAGE_SEVERITY_FATAL
# define AUTK_FATAL(...) AUTK_TOPLEVEL autk_fatal_message_nop_f(__VA_ARGS__)
#elif AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_FATAL(...) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_fatal_message_f(AUTK_MODULE_NAME, &_autk_location_, __VA_ARGS__); \
     } while (0)
#else
# define AUTK_FATAL(...) AUTK_TOPLEVEL autk_fatal_message_f(AUTK_MODULE_NAME, NULL, __VA_ARGS__)
#endif

/// \def AUTK_ASSERT
#if AUTK_ENABLE_ASSERT
# if AUTK_ENABLE_SOURCE_LOCATION
#  define AUTK_ASSERT(expr) \
      do { \
          static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                               __LINE__}; \
          if (!(expr)) { \
              AUTK_TOPLEVEL autk_assert_failure(AUTK_MODULE_NAME, &_autk_location_, #expr); \
          } \
      } while (0)
# else
#  define AUTK_ASSERT(expr) \
      do { \
          if (!(expr)) { \
              AUTK_TOPLEVEL autk_assert_failure(AUTK_MODULE_NAME, NULL, #expr); \
          } \
      } while (0)
# endif
#else
# define AUTK_ASSERT(expr) ((void)sizeof(expr))
#endif

/// \def AUTK_EXPECT
#if AUTK_ENABLE_SOURCE_LOCATION
# define AUTK_EXPECT(expr) \
     do { \
         static const AUTK_TOPLEVEL autk_source_location_t _autk_location_ = {__func__, __FILE__, \
                                                                              __LINE__}; \
         AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr); \
         if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) { \
             AUTK_TOPLEVEL autk_expect_failure(AUTK_MODULE_NAME, &_autk_location_, #expr, \
                                               _autk_status_); \
         } \
     } while (0)
#else
# define AUTK_EXPECT(expr) \
     do { \
         AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr); \
         if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) { \
             AUTK_TOPLEVEL autk_expect_failure(AUTK_MODULE_NAME, NULL, #expr, _autk_status_); \
         } \
     } while (0)
#endif

/// \def AUTK_TRY
#define AUTK_TRY(expr) \
    do { \
        AUTK_TOPLEVEL autk_status_t _autk_status_ = (expr); \
        if (_autk_status_ != AUTK_TOPLEVEL AUTK_OK) { \
            return _autk_status_; \
        } \
    } while (0)

typedef uint8_t autk_console_mode_t; ///< \see \ref autk_console_mode
typedef int8_t autk_message_severity_t;

typedef struct autk_source_location autk_source_location_t;

typedef void (*autk_message_func_t)(void *ctx, autk_message_severity_t severity,
                                    const char *module_name, const autk_source_location_t *location,
                                    const char *message);
typedef bool (*autk_message_filter_func_t)(void *ctx, autk_message_severity_t severity,
                                           const char *module_name);

enum autk_console_mode {
    AUTK_CONSOLE_MODE_ATTACH = 1,
    AUTK_CONSOLE_MODE_ALLOCATE = 2,
};

struct autk_source_location {
    const char *func_name;
    const char *file_name;
    uint32_t line_num;
};

AUTK_BEGIN_DECLS

AUTK_API AUTK_NORETURN void
autk_assert_failure(const char *module_name, const autk_source_location_t *location,
                    const char *expr);

AUTK_API autk_status_t
autk_console_init(autk_console_mode_t mode);

AUTK_API AUTK_NORETURN void
autk_expect_failure(const char *module_name, const autk_source_location_t *location,
                    const char *expr, autk_status_t status);

AUTK_API AUTK_NORETURN void
autk_fatal_exit(void);

AUTK_API AUTK_NORETURN void
autk_fatal_message(const char *module_name, const autk_source_location_t *location,
                   const char *message);

AUTK_PRINTFLIKE(3, 4) AUTK_API AUTK_NORETURN void
autk_fatal_message_f(const char *module_name, const autk_source_location_t *location,
                     const char *AUTK_FMTSTR fmt, ...);

AUTK_PRINTFLIKE(3, 0) AUTK_API AUTK_NORETURN void
autk_fatal_message_vf(const char *module_name, const autk_source_location_t *location,
                      const char *AUTK_FMTSTR fmt, va_list args);

AUTK_API void
autk_message(autk_message_severity_t severity, const char *module_name,
             const autk_source_location_t *location, const char *message);

AUTK_PRINTFLIKE(4, 5) AUTK_API autk_status_t
autk_message_f(autk_message_severity_t severity, const char *module_name,
               const autk_source_location_t *location, const char *AUTK_FMTSTR fmt, ...);

AUTK_PRINTFLIKE(4, 0) AUTK_API autk_status_t
autk_message_vf(autk_message_severity_t severity, const char *module_name,
                const autk_source_location_t *location, const char *AUTK_FMTSTR fmt, va_list args);

AUTK_API void
autk_message_default(void *unused_ctx, autk_message_severity_t severity, const char *module_name,
                     const autk_source_location_t *location, const char *message);

AUTK_API bool
autk_message_default_filter(void *unused_ctx, autk_message_severity_t severity,
                            const char *module_name);

AUTK_API void
autk_set_message_handler(autk_message_func_t func, autk_message_filter_func_t filter_func,
                         void *ctx);

AUTK_API void
autk_set_min_message_severity(autk_message_severity_t severity);

AUTK_API char *
autk_status_to_string(autk_status_t status, char *buf, size_t buf_size);

static AUTK_NORETURN AUTK_ALWAYS_INLINE void
autk_fatal_message_nop_f(AUTK_UNUSED const char *AUTK_FMTSTR fmt, ...)
{
    autk_fatal_exit();
}

static AUTK_ALWAYS_INLINE void
autk_message_nop_f(AUTK_UNUSED const char *AUTK_FMTSTR fmt, ...)
{
}

AUTK_END_DECLS

#endif // AUTK_MESSAGE_H_
