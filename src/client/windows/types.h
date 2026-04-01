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

#ifndef AUTK_CLIENT_WINDOWS_TYPES_H_
#define AUTK_CLIENT_WINDOWS_TYPES_H_

#include <windows.h>

#include <core/types.h>

typedef struct autk_windows_client_data autk_windows_client_data_t;
typedef struct autk_windows_window_data autk_windows_window_data_t;

struct autk_windows_client_data {
    int _dummy;
};

struct autk_windows_window_data {
    HWND hwnd;
};

#endif // AUTK_CLIENT_WINDOWS_TYPES_H_
