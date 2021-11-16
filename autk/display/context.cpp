/*
 * Copyright (c) Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdexcept>

#include <autk/display/context.h>
#include "object_data.h"

#if AUTK_DISPLAY_X11
# include <autk/display/x11/context.h>
#endif

using namespace autk;

DisplayContext::DisplayContext()
    : DisplayContext{std::make_unique<Data>()}
{
}

DisplayContext::~DisplayContext()
{
}

std::unique_ptr<DisplayContext> DisplayContext::open_default()
{
#if AUTK_UNIX
# if AUTK_DISPLAY_X11
    return std::make_unique<X11DisplayContext>();
# endif
#else
# error "Not implemented"
#endif
}
