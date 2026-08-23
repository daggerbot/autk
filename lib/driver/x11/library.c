// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <autk/diagnostics.h>

#include "../../os/posix/dylib.h"
#include "library.h"

#if AUTK_X11_DLOPEN

AUTK_HIDDEN autk_xcb_procs_t autk_xcb_procs = {0};

static const autk_dylib_symbol_t xcb_symbols[] = {
# define AUTK_DO_(IDENT, RETURN, ...) {#IDENT, (void **)&autk_xcb_procs.IDENT},
    AUTK_FOR_EACH_XCB_FUNC(AUTK_DO_)
# undef AUTK_DO_
};

static autk_dylib_t xcb_dylib = {
    .lib_name = "libxcb.so.1",
    .symbols = xcb_symbols,
    .symbol_count = AUTK_LENGTHOF(xcb_symbols),
};

#endif // AUTK_X11_DLOPEN

AUTK_HIDDEN autk_status_t
autk_load_x11_libs(void)
{
#if AUTK_X11_DLOPEN
    AUTK_TRY(autk_dylib_load(&xcb_dylib));
#endif
    return AUTK_OK;
}
