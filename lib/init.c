// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <autk/init.h>

#include "driver.h"

#if AUTK_X11
# include "driver/x11/init.h"
#endif

AUTK_HIDDEN const autk_driver_procs_t *autk_d = NULL;

AUTK_API autk_status_t
autk_init(void)
{
    if (!autk_d) {
#if AUTK_X11
        autk_d = &autk_x11_driver_procs;
#endif
    }

    if (autk_d) {
        if (autk_d->init) {
            return autk_d->init();
        } else {
            return AUTK_OK;
        }
    } else {
        return AUTK_ERR_NO_DRIVER;
    }
}

AUTK_API void
autk_shutdown(void)
{
    if (autk_d && autk_d->shutdown) {
        autk_d->shutdown();
    }
}
