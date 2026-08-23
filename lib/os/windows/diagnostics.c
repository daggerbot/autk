// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <stdio.h>
#include <windows.h>

#include "../../diagnostics_internal.h"

typedef struct windows_message_state windows_message_state_t;

struct windows_message_state {
    CRITICAL_SECTION critical_section;
    HANDLE stderr_handle;
    autk_message_flags_t message_flags;
    bool critical_section_was_init;
};

static windows_message_state_t s_windows_message_state = {0};

AUTK_API autk_status_t
autk_console_init(autk_console_mode_t mode)
{
    DWORD mode_flags;

    switch (mode) {
        case AUTK_CONSOLE_MODE_ATTACH:
        case AUTK_CONSOLE_MODE_ALLOCATE:
            break;
        default:
            return AUTK_ERR_INVALID_ENUM;
    }

    if (!s_windows_message_state.critical_section_was_init) {
        InitializeCriticalSection(&s_windows_message_state.critical_section);
        s_windows_message_state.critical_section_was_init = true;
    }

    if (!s_windows_message_state.stderr_handle) {
        switch (mode) {
            case AUTK_CONSOLE_MODE_ATTACH:
                AttachConsole(ATTACH_PARENT_PROCESS);
                break;
            case AUTK_CONSOLE_MODE_ALLOCATE:
                AllocConsole();
                break;
            default:
                break;
        }
    }

    s_windows_message_state.stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
    if (s_windows_message_state.stderr_handle == INVALID_HANDLE_VALUE) {
        s_windows_message_state.stderr_handle = NULL;
    }

    if (s_windows_message_state.stderr_handle
        && GetConsoleMode(s_windows_message_state.stderr_handle, &mode_flags))
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleMode(s_windows_message_state.stderr_handle,
                       ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT
                           | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        _wfreopen(L"CONOUT$", L"w", stderr);
        s_windows_message_state.message_flags = AUTK_MESSAGE_ANSI_ESCAPES;
    } else {
        s_windows_message_state.message_flags = 0;
    }

    return AUTK_OK;
}

AUTK_API void
autk_message_default(AUTK_UNUSED void *ctx, autk_message_severity_t severity,
                     const char *module_name, const autk_source_location_t *location,
                     const char *message)
{
    EnterCriticalSection(&s_windows_message_state.critical_section);
    autk_message_stderr_impl(s_windows_message_state.message_flags, severity, module_name, location,
                             message);
    LeaveCriticalSection(&s_windows_message_state.critical_section);
}

AUTK_API bool
autk_message_default_filter(AUTK_UNUSED void *ctx, AUTK_UNUSED autk_message_severity_t severity,
                            AUTK_UNUSED const char *module_name)
{
    return s_windows_message_state.stderr_handle != NULL;
}
