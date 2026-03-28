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

#ifndef AUTK_CLIENT_WINDOWS_WINDOW_H_
#define AUTK_CLIENT_WINDOWS_WINDOW_H_

#include <windows.h>

#include <autk/window.h>

#define AUTK_WINDOWS_WINDOW_CLASS_NAME L"autk_window"

typedef struct autk_windows_window_data {
    HWND hwnd;
} autk_windows_window_data_t;

AUTK_HIDDEN LRESULT CALLBACK
autk_windows_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

AUTK_HIDDEN extern const autk_window_driver_t autk_window_driver_windows;

#endif // AUTK_CLIENT_WINDOWS_WINDOW_H_
