// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_DRIVER_WINDOWS_INIT_H_
#define AUTK_DRIVER_WINDOWS_INIT_H_

#include "../../driver.h"

AUTK_HIDDEN extern const autk_driver_procs_t autk_win32_driver_procs;

AUTK_HIDDEN autk_status_t
autk_win32_init(void);

AUTK_HIDDEN void
autk_win32_shutdown(void);

#endif // AUTK_DRIVER_WINDOWS_INIT_H_
