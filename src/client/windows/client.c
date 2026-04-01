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

#include <windows.h>

#include <autk/diagnostics.h>
#include <core/types.h>
#include <os/windows/system.h>

#include "window.h"

static autk_status_t
register_window_class(autk_instance_t *instance)
{
    HINSTANCE hInstance = GetModuleHandleW(NULL);
    char errbuf[256];
    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = &autk_windows_window_proc,
        .hInstance = hInstance,
        .lpszClassName = AUTK_WINDOWS_WINDOW_CLASS_NAME,
    };

    if (!hInstance) {
        AUTK_ERROR(instance, "GetModuleHandleW failed: %s",
                   autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    window_class.hCursor = LoadCursorW(NULL, IDC_ARROW);
    if (!window_class.hCursor) {
        AUTK_ERROR(instance, "LoadCursorW failed: %s",
                   autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    if (!RegisterClassExW(&window_class)) {
        AUTK_ERROR(instance, "RegisterClassExW failed: %s",
                   autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
        return AUTK_ERR_RUNTIME_FAILURE;
    }

    return AUTK_OK;
}

//==============================================================================
//
// Client driver
//
//==============================================================================

static autk_status_t
autk_windows_client_init(autk_client_t *client, void *opaque_client_data,
                         const autk_client_create_params_t *params)
{
    (void)opaque_client_data;
    (void)params;

    AUTK_TRY(register_window_class(client->instance));

    return AUTK_OK;
}

static autk_status_t
autk_window_client_run(autk_client_t *client, void *opaque_client_data)
{
    char errbuf[256];
    MSG msg;
    BOOL msg_result;

    (void)client;
    (void)opaque_client_data;

    while (1) {
        msg_result = GetMessageW(&msg, NULL, 0, 0);
        if (msg_result == -1) {
            AUTK_ERROR(client->instance, "GetMessageW failed: %s",
                       autk_windows_error_to_string(GetLastError(), errbuf, sizeof(errbuf)));
            return AUTK_ERR_RUNTIME_FAILURE;
        } else if (!msg_result) {
            break;
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return AUTK_OK;
}

static autk_status_t
autk_window_client_quit(autk_client_t *client, void *opaque_client_data)
{
    (void)client;
    (void)opaque_client_data;

    PostQuitMessage(0);

    return AUTK_OK;
}

AUTK_API const autk_client_driver_t autk_client_driver_windows = {
    .struct_size = sizeof(autk_client_driver_t),
    .driver_data_size = sizeof(autk_windows_client_data_t),

    .window_driver = &autk_window_driver_windows,

    .init = &autk_windows_client_init,
    .run = &autk_window_client_run,
    .quit = &autk_window_client_quit,
};
