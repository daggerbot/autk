// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#ifndef AUTK_DRIVER_X11_LIBRARY_H_
#define AUTK_DRIVER_X11_LIBRARY_H_

#include <xcb/xcb.h>

#include <autk/types.h>

#if AUTK_X11_DLOPEN
# define CALL_XCB(expr) (autk_xcb_procs.expr)

/* clang-format off */
#define AUTK_FOR_EACH_XCB_FUNC(m) \
    m(xcb_connect, xcb_connection_t *, const char *, int *) \
    m(xcb_connection_has_error, int, xcb_connection_t *) \
    m(xcb_disconnect, void, xcb_connection_t *)
/* clang-format on */

typedef struct autk_xcb_procs autk_xcb_procs_t;

struct autk_xcb_procs {
# define AUTK_DO_(IDENT, RETURN, ...) RETURN (*IDENT)(__VA_ARGS__);
    AUTK_FOR_EACH_XCB_FUNC(AUTK_DO_)
# undef AUTK_DO_
};

AUTK_HIDDEN extern autk_xcb_procs_t autk_xcb_procs;

#else // !AUTK_X11_DLOPEN
# define CALL_XCB(expr) (expr)
#endif // !AUTK_X11_DLOPEN

AUTK_HIDDEN autk_status_t
autk_load_x11_libs(void);

#endif // AUTK_DRIVER_X11_LIBRARY_H_
