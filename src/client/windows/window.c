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
#include <windows.h>

#include <autk/diagnostics.h>

#include "../../impl_math.h"
#include "../../impl_types.h"
#include "../../platform/windows/system.h"
#include "window.h"

//==============================================================================
//
// Window message handler
//
//==============================================================================

AUTK_HIDDEN LRESULT CALLBACK
autk_windows_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    autk_window_t *window;
    autk_windows_window_data_t *window_data;

    switch (msg) {
        case WM_CLOSE:
            window = (autk_window_t *)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (window && window->callbacks && window->callbacks->close_requested) {
                window->callbacks->close_requested(window, window->user_data);
            }
            return 0;

        case WM_DESTROY:
            window = (autk_window_t *)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (window) {
                window_data = window->driver_data;
                if (window_data->hwnd) {
                    // If we reach here, the window was destroyed by the window system, not by us.
                    // Invalidate the autk_window_t but don't free it.
                    if (window->callbacks && window->callbacks->invalidated) {
                        window->callbacks->invalidated(window, window->user_data);
                    }
                    window_data->hwnd = NULL;
                }
            }
            return 0;

        default:
            return DefWindowProcW(hwnd, msg, wparam, lparam);
    }
}

//==============================================================================
//
// Window driver
//
//==============================================================================

static autk_status_t
autk_windows_window_init(autk_window_t *window, void *opaque_window_data,
                         const autk_window_create_params_t *params)
{
    char errbuf[256];
    autk_windows_window_data_t *window_data = opaque_window_data;
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD ex_style = 0;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    DWORD error_code;

    if (!hInstance) {
        AUTK_ERROR(window->instance, "GetModuleHandleW failed: %s",
                   autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    // Determine final window style.
    switch (params->type) {
        case AUTK_WINDOW_TYPE_NORMAL:
            break;
        default:
            return AUTK_ERR_INVALID_ARGUMENT;
    }

    // Determine the final position of the window.
    if (params->x || params->y || (params->flags & AUTK_WINDOW_CREATE_FLAGS_EXPLICIT_POSITION)) {
        x = params->x;
        y = params->y;
    }

    // Determine the final size of the window.
    if (params->width || params->height) {
        RECT rect = {0, 0, (int)autk_uint32_clamp(params->width, 1, INT_MAX),
                     (int)autk_uint32_clamp(params->height, 1, INT_MAX)};

        if (!AdjustWindowRectEx(&rect, style, FALSE, ex_style)) {
            AUTK_ERROR(window->instance, "AdjustWindowRectEx failed: %s",
                       autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
            return AUTK_ERR_RUNTIME_FAILURE;
        }

        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

    // Create the window.
    window_data->hwnd = CreateWindowExW(ex_style, AUTK_WINDOWS_WINDOW_CLASS_NAME, NULL, style, x, y,
                                        width, height, NULL, NULL, hInstance, NULL);
    if (!window_data->hwnd) {
        AUTK_ERROR(window->instance, "CreateWindowExW failed: %s",
                   autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    // Point the HWND back to our autk_window_t object.
    SetLastError(0);
    if (!SetWindowLongPtrW(window_data->hwnd, GWLP_USERDATA, (LONG_PTR)window)) {
        error_code = GetLastError();
        if (error_code) {
            AUTK_ERROR(window->instance, "SetWindowLongPtrW failed: %s",
                       autk_windows_error_to_string(error_code, errbuf, sizeof(errbuf)));
            return AUTK_ERR_RUNTIME_FAILURE;
        }
    }

    return AUTK_OK;
}

static void
autk_windows_window_fini(autk_window_t *window, void *opaque_window_data)
{
    autk_windows_window_data_t *window_data = opaque_window_data;
    HWND hwnd;

    (void)window;

    if (window_data->hwnd) {
        hwnd = window_data->hwnd;
        window_data->hwnd = NULL; // Prevent calling `callbacks->invalidated`.
        DestroyWindow(hwnd);
    }
}

static autk_status_t
autk_windows_window_set_visible(autk_window_t *window, void *opaque_window_data, bool visible)
{
    autk_windows_window_data_t *window_data = opaque_window_data;

    (void)window;

    if (window_data->hwnd) {
        ShowWindow(window_data->hwnd, visible ? SW_SHOW : SW_HIDE);
    } else if (visible) {
        return AUTK_ERR_RESOURCE_LOST;
    }

    return AUTK_OK;
}

AUTK_HIDDEN const autk_window_driver_t autk_window_driver_windows = {
    .struct_size = sizeof(autk_window_driver_t),
    .driver_data_size = sizeof(autk_windows_window_data_t),

    .init = &autk_windows_window_init,
    .fini = &autk_windows_window_fini,
    .set_visible = &autk_windows_window_set_visible,
};
