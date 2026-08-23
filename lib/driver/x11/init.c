// Copyright (c) 2026 Martin Mills
// SPDX-License-Identifier: ISC
// See LICENSE.txt for more information.

#include <stdlib.h>

#include <autk/diagnostics.h>

#include "init.h"
#include "library.h"

AUTK_HIDDEN autk_x11_global_t autk_x11 = {0};

AUTK_HIDDEN const autk_driver_procs_t autk_x11_driver_procs = {
    .init = autk_x11_init,
    .shutdown = autk_x11_shutdown,
};

static autk_status_t
check_connection(void)
{
    int error_code = CALL_XCB(xcb_connection_has_error(autk_x11.connection));

    switch (error_code) {
        case 0:
            return AUTK_OK;
        case XCB_CONN_ERROR:
            AUTK_ERROR("xcb: Socket or connection error");
            return AUTK_ERR_IO;
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
            AUTK_ERROR("xcb: Unsupported extension");
            return AUTK_ERR_INVALID_CONFIG;
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
            AUTK_ERROR("xcb: Out of memory");
            return AUTK_ERR_OUT_OF_MEMORY;
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
            AUTK_ERROR("xcb: Request length exceeded");
            return AUTK_ERR_PROTOCOL;
        case XCB_CONN_CLOSED_PARSE_ERR:
            AUTK_ERROR("xcb: Invalid display name");
            return AUTK_ERR_INVALID_CONFIG;
        case XCB_CONN_CLOSED_INVALID_SCREEN:
            AUTK_ERROR("xcb: Invalid screen index");
            return AUTK_ERR_INVALID_CONFIG;
        case XCB_CONN_CLOSED_FDPASSING_FAILED:
            AUTK_ERROR("xcb: fd passing failed");
            return XCB_CONN_CLOSED_FDPASSING_FAILED;
        default:
            AUTK_ERROR("xcb: Error code %d", error_code);
            return AUTK_ERR_IO;
    }
}

AUTK_HIDDEN autk_status_t
autk_x11_init(void)
{
    AUTK_TRY(autk_load_x11_libs());

    if (!autk_x11.display_name) {
        autk_x11.display_name = getenv("DISPLAY");
        if (!autk_x11.display_name || !autk_x11.display_name[0]) {
            autk_x11.display_name = ":0";
        }
    }

    if (!autk_x11.connection) {
        AUTK_INFO("Connecting to X11 display: %s", autk_x11.display_name);
        // xcb_connect returns a dummy pointer on failure, not null.
        autk_x11.connection = CALL_XCB(xcb_connect(NULL, &autk_x11.default_screen_index));
        AUTK_TRY(check_connection());
    }

    return AUTK_OK;
}

AUTK_HIDDEN void
autk_x11_shutdown(void)
{
    if (autk_x11.connection) {
        CALL_XCB(xcb_disconnect(autk_x11.connection));
        autk_x11.connection = NULL;
    }

    autk_x11.display_name = NULL;
}
