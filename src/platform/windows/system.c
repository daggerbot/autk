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

#include <limits.h>
#include <stdio.h>
#include <windows.h>

#include "../../os_compat.h"
#include "system.h"

AUTK_HIDDEN char *
autk_windows_error_to_string(DWORD error_code, char *buf, size_t buf_size)
{
    wchar_t wbuf[256];
    DWORD result;

    // Format the message as UTF-16. We can't rely on FormatMessageA giving us an ASCII or UTF-8
    // string, so we use the wide version and convert it ourselves.
    result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                            error_code, 0, wbuf, sizeof(wbuf) / sizeof(wchar_t), NULL);
    if (result == 0) {
        goto fallback;
    }

    // Convert the message to UTF-8.
    if (buf_size > (size_t)INT_MAX) {
        buf_size = (size_t)INT_MAX;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, (int)buf_size, NULL, NULL) <= 0) {
        goto fallback;
    }

    // Success!
    return buf;

fallback:
    if (snprintf(buf, buf_size, "Win32 error code %u", (unsigned int)error_code) < 0) {
        autk_strlcpy(buf, "Unknown Win32 error", buf_size);
    }
    return buf;
}
