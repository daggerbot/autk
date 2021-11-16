/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "library.h"

using namespace autk;

//==============================================================================
// X11Library
//==============================================================================

X11Library::X11Library()
    : lib{"libX11.so.6"}
#define DO(NAME, ...) , NAME{reinterpret_cast<FunctionPointer<__VA_ARGS__>>(lib.get_proc_address(#NAME))}
    AUTK_FOREACH_X11_SYMBOL(DO)
#undef DO
{
}

const X11Library& X11Library::open()
{
    static X11Library lib;
    return lib;
}

//==============================================================================
// X11XcbLibrary
//==============================================================================

X11XcbLibrary::X11XcbLibrary()
    : lib{"libX11-xcb.so.1"}
#define DO(NAME, ...) , NAME{reinterpret_cast<FunctionPointer<__VA_ARGS__>>(lib.get_proc_address(#NAME))}
    AUTK_FOREACH_X11_XCB_SYMBOL(DO)
#undef DO
{
}

const X11XcbLibrary& X11XcbLibrary::open()
{
    static X11XcbLibrary lib;
    return lib;
}
