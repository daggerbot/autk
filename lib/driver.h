// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_DRIVER_H_
#define AUTK_DRIVER_H_

#include <autk/types.h>

typedef struct autk_driver_procs autk_driver_procs_t;

struct autk_driver_procs {
    autk_status_t (*init)(void);
    void (*shutdown)(void);
};

AUTK_HIDDEN extern const autk_driver_procs_t *autk_d;

#endif // AUTK_DRIVER_H_
