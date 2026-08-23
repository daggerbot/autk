// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <dlfcn.h>

#include <autk/diagnostics.h>

#include "dylib.h"

AUTK_HIDDEN autk_status_t
autk_dylib_load(autk_dylib_t *dylib)
{
    const char *reason;
    void *proc;

    if (dylib->loaded) {
        return AUTK_OK;
    }

    AUTK_INFO("Loading library: %s", dylib->lib_name);

    dylib->dlhandle = dlopen(dylib->lib_name, RTLD_LAZY | RTLD_GLOBAL);
    if (!dylib->dlhandle) {
        reason = dlerror();
        AUTK_ERROR("%s: dlopen failed: %s", dylib->lib_name, reason ? reason : "Unknown reason");
        return AUTK_ERR_DYNAMIC_LINK;
    }

    for (size_t i = 0; i < dylib->symbol_count; i++) {
        proc = dlsym(dylib->dlhandle, dylib->symbols[i].name);
        if (!proc) {
            AUTK_ERROR("%s: missing symbol: %s", dylib->lib_name, dylib->symbols[i].name);
            return AUTK_ERR_DYNAMIC_LINK;
        }
        *dylib->symbols[i].ptr = proc;
    }

    dylib->loaded = true;
    return AUTK_OK;
}
