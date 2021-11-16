/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <autk/display/x11/context.h>
#include <autk/display/x11/error.h>
#include "object_data.h"

using namespace autk;

//==============================================================================
// X11DisplayContext
//==============================================================================

X11DisplayContext::X11DisplayContext()
    : X11DisplayContext{nullptr}
{
}

X11DisplayContext::X11DisplayContext(const char* name)
    : X11DisplayContext{std::make_unique<Data>()}
{
    const auto& libx11 = data().libx11;
    const auto& libx11xcb = data().libx11xcb;

    data().xlib.reset(libx11.XOpenDisplay(name));
    auto xlib = data().xlib.get();
    if (!xlib) {
        if (name) {
            throw X11Error{std::string{name} + ": " + "Can't open X display connection"};
        } else {
            throw X11Error{"Can't open X display connection"};
        }
    }

    libx11xcb.XSetEventQueueOwner(xlib, XCBOwnsEventQueue);
    data().xcb = libx11xcb.XGetXCBConnection(xlib);
    auto xcb = data().xcb;
    if (!xcb) {
        throw X11Error{"Can't get XCB connection from Xlib connection"};
    }
}

X11DisplayContext::~X11DisplayContext()
{
    // Invoke this signal early to ensure that the connection is still valid in case some
    // handler tries to use it again.
    data().sig_disposed(*this);
}

xcb_connection_t* X11DisplayContext::get_xcb_connection() const
{
    return data().xcb;
}

Display* X11DisplayContext::get_xlib_display_ptr() const
{
    return data().xlib.get();
}
