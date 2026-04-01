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

#undef _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>

#include "compat.h"

AUTK_API char *
autk_strerror_r(int errnum, char *buf, size_t buf_size)
{
#ifdef __unix__
    if (strerror_r(errnum, buf, buf_size) == 0) {
        return buf;
    }
#elif defined(_WIN32)
    // TODO: _wcserror_s and convert to UTF-8?
#endif

    if (snprintf(buf, buf_size, "errno = %d", errnum) >= 0) {
        return buf;
    }

    autk_strlcpy(buf, "errno formatting failed", buf_size);
    return buf;
}

AUTK_API size_t
autk_strlcpy(char *AUTK_RESTRICT buf, const char *AUTK_RESTRICT src, size_t buf_size)
{
    size_t i = 0;

    if (buf_size > 0) {
        while (i < buf_size - 1 && src[i]) {
            buf[i] = src[i];
            i++;
        }
        buf[i] = 0;
    }

    while (src[i]) {
        i++;
    }

    return i;
}
