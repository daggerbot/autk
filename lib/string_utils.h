// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_STRING_UTILS_H_
#define AUTK_STRING_UTILS_H_

#include <autk/types.h>

typedef uint32_t autk_fmt_flags_t; ///< \see \ref autk_fmt_flags

enum autk_fmt_flags {
    AUTK_FMT_ALLOCATE = 1 << 0,
    AUTK_FMT_ALLOW_TRUNCATE = 1 << 1,
};

AUTK_PRINTFLIKE(4, 5) AUTK_HIDDEN autk_status_t
autk_call_with_formatted_string_f(autk_fmt_flags_t flags,
                                  void (*callback)(void *ctx, const char *str, size_t len),
                                  void *ctx, const char *AUTK_FMTSTR fmt, ...);

AUTK_PRINTFLIKE(4, 0) AUTK_HIDDEN autk_status_t
autk_call_with_formatted_string_vf(autk_fmt_flags_t flags,
                                   void (*callback)(void *ctx, const char *str, size_t len),
                                   void *ctx, const char *AUTK_FMTSTR fmt, va_list args);

AUTK_HIDDEN char *
autk_strcpy_max(char *AUTK_RESTRICT buf, const char *AUTK_RESTRICT src, size_t buf_size);

#endif // AUTK_STRING_UTILS_H_
