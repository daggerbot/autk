// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_DRIVER_X11_INIT_H_
#define AUTK_DRIVER_X11_INIT_H_

#include <xcb/xcb.h>

#include <autk/types.h>

#include "../../driver.h"

typedef struct autk_x11_global autk_x11_global_t;

struct autk_x11_global {
    const char *display_name;
    xcb_connection_t *connection;
    int default_screen_index;
};

AUTK_HIDDEN extern autk_x11_global_t autk_x11;
AUTK_HIDDEN extern const autk_driver_procs_t autk_x11_driver_procs;

AUTK_HIDDEN autk_status_t
autk_x11_init(void);

AUTK_HIDDEN void
autk_x11_shutdown(void);

#endif // AUTK_DRIVER_X11_INIT_H_
