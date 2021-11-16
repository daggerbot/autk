/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_X11_CONTEXT_H_INCLUDED
#define AUTK_DISPLAY_X11_CONTEXT_H_INCLUDED

#include <X11/Xlib.h>
#include <xcb/xcb.h>

#include "../context.h"

namespace autk {

    /// \addtogroup display
    /// @{

    /// X11 display connection.
    class AUTK_DISPLAY X11DisplayContext : public DisplayContext {
    public:
        /// Connects to the default X11 display.
        X11DisplayContext();

        /// Connects to the specified X11 display.
        explicit X11DisplayContext(const char* name);

        ~X11DisplayContext();

        /// Returns a pointer to the XCB display connection object.
        xcb_connection_t* get_xcb_connection() const;

        /// Returns a pointer to the Xlib display connection object.
        /// Autk primarily uses XCB rather than Xlib, but Xlib is still required in order to fully support GLX.
        Display* get_xlib_display_ptr() const;

    private:
        struct Data;

        explicit X11DisplayContext(std::unique_ptr<Data>&& data);

        Data& data();
        const Data& data() const;
    };

    /// @}

} // namespace autk

#endif // AUTK_DISPLAY_X11_CONTEXT_H_INCLUDED
