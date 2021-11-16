/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef AUTK_DISPLAY_X11_OBJECT_DATA_H_INCLUDED
#define AUTK_DISPLAY_X11_OBJECT_DATA_H_INCLUDED

#include <autk/display/x11/context.h>
#include "../object_data.h"
#include "library.h"

namespace autk {

    //==============================================================================
    // X11DisplayContext
    //==============================================================================

    struct AUTK_DISPLAY X11DisplayContext::Data : DisplayContext::Data {
        const X11Library& libx11;
        const X11XcbLibrary& libx11xcb;

        std::unique_ptr<Display, void (*)(Display*)> xlib;
        xcb_connection_t* xcb = nullptr;

        Data()
            : libx11{X11Library::open()},
              libx11xcb{X11XcbLibrary::open()},
              xlib{nullptr, libx11.XCloseDisplay}
        {
        }
    };

    inline X11DisplayContext::X11DisplayContext(std::unique_ptr<Data>&& data)
        : DisplayContext{std::move(data)}
    {
    }

    inline X11DisplayContext::Data& X11DisplayContext::data()
    {
        return static_cast<Data&>(DisplayContext::data());
    }

    inline const X11DisplayContext::Data& X11DisplayContext::data() const
    {
        return static_cast<const Data&>(DisplayContext::data());
    }

} // namespace autk

#endif // AUTK_DISPLAY_X11_OBJECT_DATA_H_INCLUDED
