/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_X11_LIBRARY_H_INCLUDED
#define AUTK_DISPLAY_X11_LIBRARY_H_INCLUDED

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#include <autk/core/dynamic_library.h>

#define AUTK_FOREACH_X11_SYMBOL(m) \
    m(XCloseDisplay, void, Display*) \
    m(XOpenDisplay, Display*, const char*)

#define AUTK_FOREACH_X11_XCB_SYMBOL(m) \
    m(XGetXCBConnection, xcb_connection_t*, Display*) \
    m(XSetEventQueueOwner, void, Display*, enum XEventQueueOwner)

namespace autk {

    class AUTK_DISPLAY X11Library {
    private:
        DynamicLibrary lib;

        X11Library();
        X11Library(X11Library&&) = delete;
        X11Library(const X11Library&) = delete;

        X11Library& operator=(X11Library&&) = delete;
        X11Library& operator=(const X11Library&) = delete;

    public:
#define DO(NAME, ...) const FunctionPointer<__VA_ARGS__> NAME = nullptr;
        AUTK_FOREACH_X11_SYMBOL(DO)
#undef DO

        static const X11Library& open();
    };

    class AUTK_DISPLAY X11XcbLibrary {
    private:
        DynamicLibrary lib;

        X11XcbLibrary();
        X11XcbLibrary(X11XcbLibrary&&) = delete;
        X11XcbLibrary(const X11XcbLibrary&) = delete;

        X11XcbLibrary& operator=(X11XcbLibrary&&) = delete;
        X11XcbLibrary& operator=(const X11XcbLibrary&) = delete;

    public:
#define DO(NAME, ...) const FunctionPointer<__VA_ARGS__> NAME = nullptr;
        AUTK_FOREACH_X11_XCB_SYMBOL(DO)
#undef DO

        static const X11XcbLibrary& open();
    };

} // namespace autk

#endif // AUTK_DISPLAY_X11_LIBRARY_H_INCLUDED
