// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include "init.h"

AUTK_HIDDEN const autk_driver_procs_t autk_win32_driver_procs = {
    .init = autk_win32_init,
    .shutdown = autk_win32_shutdown,
};

AUTK_HIDDEN autk_status_t
autk_win32_init(void)
{
    return AUTK_OK;
}

AUTK_HIDDEN void
autk_win32_shutdown(void)
{
}
