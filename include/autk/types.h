// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_TYPES_H_
#define AUTK_TYPES_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "base.h"

typedef uint32_t autk_status_t; ///< \see \ref autk_status

/* clang-format off */
#define AUTK_FOR_EACH_STATUS(m) \
    m(AUTK_OK, "No error") \
    m(AUTK_ERR_DYNAMIC_LINK, "Dynamic library linking failed") \
    m(AUTK_ERR_INCONSISTENT_STATE, "Inconsistent internal state") \
    m(AUTK_ERR_INVALID_CONFIG, "Invalid configuration") \
    m(AUTK_ERR_IO, "I/O error") \
    m(AUTK_ERR_LIMIT_EXCEEDED, "Limit exceeded") \
    m(AUTK_ERR_NO_DRIVER, "No suitable Autk driver configured") \
    m(AUTK_ERR_OUT_OF_MEMORY, "Out of memory") \
    m(AUTK_ERR_PROTOCOL, "Protocol violation") \
    m(AUTK_ERR_STRING_FORMAT, "String formatting failed")
/* clang-format on */

enum autk_status {
#define AUTK_DO_(IDENT, MSG) IDENT,
    AUTK_FOR_EACH_STATUS(AUTK_DO_)
#undef AUTK_DO_
};

#endif // AUTK_TYPES_H_
